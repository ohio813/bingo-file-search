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
///:ConfigDB.h
#ifndef CONFIGDB_H
#define CONFIGDB_H

#include <QtSql>
#include <QMap>
#include <QSet>

typedef struct ConfigDBLastRecordTableNode
{
    ConfigDBLastRecordTableNode (unsigned __int64 usnJournalID, unsigned __int64 nextUsn)
    {
        UsnJournalID = usnJournalID;
        NextUsn = nextUsn;
    }
    unsigned __int64 UsnJournalID;
    unsigned __int64 NextUsn;
} ConfigDBLastRecordTableNode;

class ConfigDB
{
public:
    ConfigDB();
    ~ConfigDB()
    {
        if (m_changed)
            WriteTable ();
    }
    QMap <char, ConfigDBLastRecordTableNode> m_LastRecord;
    QSet <char> m_Disable;
    void WriteTable ();
private:
    void ReadTable (int i);
    void CreateTable (int i);
    QSqlDatabase m_db;
    QSqlQuery m_query;
    bool m_changed;
};

#endif
///:~