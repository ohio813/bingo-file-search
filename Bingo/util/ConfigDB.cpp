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
///:ConfigDB.cpp

#include "ConfigDB.h"
#include <Qt>
#include <QtCore>

ConfigDB::ConfigDB()
{
    m_changed = false;
    m_db = QSqlDatabase::addDatabase ("QSQLITE", "config_db");
    QString db_Path = QCoreApplication::applicationDirPath() + "/Data/";
    m_db.setDatabaseName (db_Path + "config.db");
    m_LastRecord.clear();
    m_Disable.clear();
ReStart:

    if (m_db.open())
    {
        m_query = QSqlQuery (m_db);
        QStringList tables = m_db.tables();

        if (tables.contains ("LastRecord"))
            ReadTable (0);
        else
            CreateTable (0);

        if (tables.contains ("Disable"))
            ReadTable (1);
        else
            CreateTable (1);

        tables = m_db.tables();
    }
    else
    {
        QDir (QCoreApplication::applicationDirPath()).mkpath (db_Path);
        goto ReStart;
    }

    m_db.close();
}

void ConfigDB::CreateTable (int i)
{
    m_query.exec ("PRAGMA encoding = \"UTF-8\";");

    switch (i)
    {
        case 0:
            m_query.exec ("create table LastRecord(Path char(1) primary key,"
                          "UsnJournalID integer(8), NextUsn integer(8))");
            break;

        case 1:
            m_query.exec ("create table Disable(Path char(1))");
            break;
    }

    m_changed = true;
}


void ConfigDB::ReadTable (int i)
{
    switch (i)
    {
        case 0:
            m_query.exec ("select * from LastRecord");

            while (m_query.next())
                m_LastRecord.insert (m_query.value (0).toString() [0].toAscii(),
                                     ConfigDBLastRecordTableNode ( (unsigned __int64) m_query.value (1).toULongLong(),
                                             (unsigned __int64) m_query.value (2).toULongLong()));

            break;

        case 1:
            m_query.exec ("select * from Disable");

            while (m_query.next())
                m_Disable.insert (m_query.value (0).toString() [0].toAscii());

            break;
    }
}

void ConfigDB::WriteTable ()
{
    if (!m_db.open())
    {
        QString db_Path = QCoreApplication::applicationDirPath() + "/Data/";
        QDir (QCoreApplication::applicationDirPath()).mkpath (db_Path);
        m_db.open();
        m_query = QSqlQuery (m_db);
        CreateTable (0);
        CreateTable (1);
    }

    m_query = QSqlQuery (m_db);
    m_db.transaction();
    m_query.exec ("delete from LastRecord");
    m_query.exec ("VACUUM;");
    m_query.prepare ("insert into LastRecord values(?,?,?)");

    for (QMap <char, ConfigDBLastRecordTableNode>::iterator ptr = m_LastRecord.begin();
            ptr != m_LastRecord.end(); ++ptr)
    {
        m_query.bindValue (0, QChar (ptr.key()));
        m_query.bindValue (1, qulonglong (ptr.value().UsnJournalID));
        m_query.bindValue (2, qulonglong (ptr.value().NextUsn));
        m_query.exec();
    }

    m_db.commit();
    m_query.exec ("delete from Disable");
    m_query.exec ("VACUUM;");
    m_query.prepare ("insert into Disable values(?)");
    foreach (char Path, m_Disable)
    {
        m_query.bindValue (0, QChar (Path));
        m_query.exec();
    }
    m_db.commit();
    m_db.close();
    m_changed = false;
}
///:~