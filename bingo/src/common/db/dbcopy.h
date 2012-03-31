/**
*   Copyright (C) 2011-2012  Xu Cheng, Yang Zhengyu, Zuo Zhiheng, Yao Wenjie
*
*   This library is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This library is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.'
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:dbcopy.h
#ifndef DBCOPY_H
#define DBCOPY_H

#include "../core/global.h"
#include <QString>
#include <QSqlDatabase>
#include <QFile>

BINGO_BEGIN_NAMESPACE

void SqlMemoryDBLoadorSave (QSqlDatabase& memdb, const QString& filedb_path, bool load)
{
    if (!load) QFile::remove (filedb_path); //drop old database

    QSqlDatabase filedb = QSqlDatabase::addDatabase ("QSQLITE", "tmp_file_db");
    filedb.setDatabaseName (filedb_path);
    filedb.open();
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
    memdb.exec (QString ("ATTACH DATABASE '%1' as source").arg (filedb_path));
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

    toDB->exec ("PRAGMA case_sensitive_like=false;");
}

void SqlDBCopy (QSqlDatabase& srcdb, QSqlDatabase& destdb)
{
    QSqlDatabase *fromDB = &srcdb;
    QSqlDatabase *toDB = &destdb;
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

    //Copy records
    toDB->exec ("BEGIN");
    fromQuery.setForwardOnly (true);
    foreach (QString tableName, tableList)
    {
        fromQuery.exec (QString ("select * from %1").arg (tableName));

        if (fromQuery.next())
        {
            int cols = fromQuery.record().count();

            if (cols != 0)
            {
                std::vector<QVariantList> rec;
                rec.resize (cols);
                QString tmp = "?";

                for (int i = 1; i < cols; ++i)
                    tmp.append (",?");

                toQuery.prepare (QString ("insert into %1 values(").arg (tableName).append (tmp).append (")"));

                for (int i = 0; i < cols; ++i)
                    rec[i] << fromQuery.value (i);

                while (fromQuery.next())
                    for (int i = 0; i < cols; ++i)
                        rec[i] << fromQuery.value (i);

                for (int i = 0; i < cols; ++i)
                    toQuery.addBindValue (rec[i]);

                toQuery.execBatch();
            }
        }
    }
    fromQuery.finish();
    toDB->exec ("COMMIT");
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

    toDB->exec ("PRAGMA case_sensitive_like=false;");
}

BINGO_END_NAMESPACE

#endif
///:~
