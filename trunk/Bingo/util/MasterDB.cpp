/**
*   Copyright (C) 2011  Xu Cheng, Yang Zhengyu ,Zuo Zhiheng
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///:MasterDB.cpp

#include "MasterDB.h"
#include <Qt>
#include <QtCore>
#include "Log.h"

bool SqlMemoryDBLoadorSave (QSqlDatabase& memdb, QSqlDatabase& filedb, bool load)
{
    if (!memdb.isOpen())
        return false;

    QString fileName = filedb.databaseName();

    if (load)
    {
        if (!filedb.isOpen())
            filedb.open();
    }
    else
    {
        //drop old database
        if (filedb.isOpen())
            filedb.close();

        QFile::remove (fileName);
        filedb.open();
    }

    QSqlDatabase *fromDB = load ? &filedb : &memdb;
    QSqlDatabase *toDB = load ? &memdb : &filedb;
    QSqlQuery fromQuery (*fromDB);
    QSqlQuery toQuery (*toDB);
    QStringList tableList;
    //Copy the tables
    fromQuery.exec ("SELECT sql, tbl_name FROM sqlite_master "
                    " WHERE type='table' AND sql NOT NULL and name NOT IN ('sqlite_stat1', 'sqlite_sequence')");

    while (fromQuery.next())
    {
        toQuery.exec (fromQuery.value (0).toString());
        tableList.push_back (fromQuery.value (1).toString());
    }

    //Attach db
    memdb.exec (QString ("ATTACH DATABASE '%1' as source").arg (fileName));
    //Copy records
    memdb.exec ("BEGIN");

    if (load)
    {
        foreach (QString tableName, tableList)
        memdb.exec (QString ("insert into main.%1 select * from source.%1").arg (tableName));
    }
    else
    {
        foreach (QString tableName, tableList)
        memdb.exec (QString ("insert into source.%1 select * from main.%1").arg (tableName));
    }

    memdb.exec ("COMMIT");
    //Detach
    memdb.exec ("DETACH DATABASE source");
    //Optimization
    toDB->exec ("PRAGMA case_sensitive_like=true;");
    toDB->exec ("PRAGMA journal_mode=MEMORY;");
    toDB->exec ("PRAGMA temp_store=MEMORY;");
    toDB->exec ("PRAGMA locking_mode=EXCLUSIVE;");
    toDB->exec ("PRAGMA synchronous = OFF;");
    toDB->exec ("PRAGMA encoding = \"UTF-8\";");
    //Copy the triggers
    fromQuery.exec ("SELECT sql FROM sqlite_master "
                    " WHERE type='trigger' AND sql NOT NULL and name NOT IN ('sqlite_stat1', 'sqlite_sequence')");

    while (fromQuery.next())
        toQuery.exec (fromQuery.value (0).toString());

    //Copy the indexes
    fromQuery.exec ("SELECT sql FROM sqlite_master "
                    " WHERE type='index' AND sql NOT NULL and name NOT IN ('sqlite_stat1', 'sqlite_sequence')");

    while (fromQuery.next())
        toQuery.exec (fromQuery.value (0).toString());

    //Copy the views
    fromQuery.exec ("SELECT sql FROM sqlite_master "
                    " WHERE type='view' AND sql NOT NULL and name NOT IN ('sqlite_stat1', 'sqlite_sequence')");

    while (fromQuery.next())
        toQuery.exec (fromQuery.value (0).toString());

    return true;
}

MasterDB::MasterDB()
{
    m_db = QSqlDatabase::addDatabase ("QSQLITE", "master_db");
    m_db.setDatabaseName (":memory:");
    m_db.open();
    m_filedb = QSqlDatabase::addDatabase ("QSQLITE", "master_file_db");
    QString db_Path = QCoreApplication::applicationDirPath() + "/Data/";
    m_filedb.setDatabaseName (db_Path + "data.db");
    SqlMemoryDBLoadorSave (m_db, m_filedb, true);
    m_query = QSqlQuery (m_db);
}
MasterDB::~MasterDB()
{
    SqlMemoryDBLoadorSave (m_db, m_filedb, false);
    m_db.close();
    m_filedb.close();
}
bool MasterDB::TableContains (char Path)
{
    return m_db.tables().contains (QString ("Master_%1").arg (Path));
}
void MasterDB::CreateTable (char Path)
{
    m_query.exec (QString ("create table Master_%1 "
                           "(frn integer(8) primary key, pfrn integer(8), name vchar,"
                           "attrib int, size int, createTime int, writeTime int)").arg (Path));
    Log::v (QString ("Create table Master_%1").arg (Path));
}
void MasterDB::DeleteTable (char Path)
{
    m_query.exec (QString ("delete from Master_%1").arg (Path));
    m_query.exec ("VACUUM;");
    Log::v (QString ("Delete table Master_%1").arg (Path));
}
void MasterDB::DropTable (char Path)
{
    m_query.exec (QString ("delete from Master_%1").arg (Path));
    m_query.exec (QString ("drop table Master_%1").arg (Path));
    m_query.exec ("VACUUM;");
    Log::v (QString ("Drop table Master_%1").arg (Path));
}
void MasterDB::EnumInsert (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                           DWORD attrib, DWORD size, TIME32 createTime, TIME32 writeTime)
{
    static char lastPath = 0;

    if (lastPath != Path)
    {
        m_query.prepare (QString ("insert into Master_%1 values(?,?,?,?,?,?,?)").arg (Path));
        lastPath = Path;
    }

    m_query.bindValue (0, qulonglong (frn));
    m_query.bindValue (1, qulonglong (pfrn));
    m_query.bindValue (2, QByteArray (utf8name.c_str()));
    m_query.bindValue (3, uint (attrib));
    m_query.bindValue (4, uint (size));
    m_query.bindValue (5, uint (createTime));
    m_query.bindValue (6, uint (writeTime));
    m_query.exec();
}
void MasterDB::ReadLastUSNDel (char Path, unsigned __int64 frn)
{
    QSqlQuery delQuery (m_query);
    delQuery.prepare (QString ("delete from Master_%1 where frn=?").arg (Path));
    delQuery.bindValue (0, qulonglong (frn));
    delQuery.exec();
}
QStringList MasterDB::getAllTables()
{
    return m_db.tables();
}
QSqlQuery* MasterDB::copyRootQuery()
{
    return new QSqlQuery (m_db);
}
///:~