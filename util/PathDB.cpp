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
///:PathDB.cpp

#include "PathDB.h"
#include <Qt>
#include <QtCore>
#include <QStack>
#include "../core/USN.h"
#include "../core/Data.h"
#include "Log.h"
#include "assert.h"

#pragma region PathDB
PathDB::PathDB()
{
    m_query = QSqlQuery (data_masterDB->getQuery());
}
void PathDB::DropTable (char Path)
{
    m_query.exec (QString ("delete from Path_%1").arg (Path));
    m_query.exec (QString ("drop table Path_%1").arg (Path));
    m_query.exec ("VACUUM;");
    Log::v (QString ("Drop table Path_%1").arg (Path));
}
#pragma endregion PathDB

#pragma region PathGen
void PathGen::setPath (char path)
{
    m_Path = path;
}
void PathGen::init()
{
    m_tmpDB = QSqlDatabase::addDatabase ("QSQLITE", QString ("path_gen_%1").arg (m_Path));
    m_tmpDB.setDatabaseName (":memory:");
    m_tmpDB.open();
    m_tmpQuery = QSqlQuery (m_tmpDB);
    m_tmpQuery.exec (QString ("create table Path_%1 "
                              "(frn integer(8) primary key, path vchar)").arg (m_Path));
    Log::v (QString ("Create table Path_%1").arg (m_Path));
    m_masterQuery = QSqlQuery (data_masterDB->getDB());
}
void PathGen::move()
{
    QSqlDatabase *fromDB = &m_tmpDB;
    QSqlDatabase *toDB = &data_masterDB->getDB();
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

    m_tmpDB.close();
}
extern bool SqlMemoryDBLoadorSave (QSqlDatabase& memdb, QSqlDatabase& filedb, bool load);
void PathGen::run()
{
    init();
    m_tmpQuery.prepare (QString ("insert into Path_%1 values (? ,'%1:')").arg (m_Path));
    m_tmpQuery.bindValue (0, qulonglong (RootFile_pfrn));
    m_tmpQuery.exec ();
    // pre-complie
    _findIndexQuery = QSqlQuery (m_masterQuery);
    _findPathQuery = QSqlQuery (m_tmpQuery);
    _existPathQuery = QSqlQuery (m_tmpQuery);
    _findIndexQuery.setForwardOnly (true);
    _findIndexQuery.prepare (QString ("select frn,pfrn,name from Master_%1 where frn=?").arg (m_Path));
    _findPathQuery.setForwardOnly (true);
    _findPathQuery.prepare (QString ("select path from Path_%1 where frn=?").arg (m_Path));
    _existPathQuery.setForwardOnly (true);
    _existPathQuery.prepare (QString ("select 1 where exists(select 1 from Path_%1 where frn=?)").arg (m_Path));
    m_tmpQuery.prepare (QString ("insert into Path_%1 values (?,?) ").arg (m_Path));
    m_masterQuery.setForwardOnly (true);
    m_masterQuery.exec (QString ("select frn,pfrn,name from Master_%1").arg (m_Path));

    while (m_masterQuery.next())
    {
        _Node _node (m_masterQuery.value (0).toULongLong(),
                     m_masterQuery.value (1).toULongLong(), m_masterQuery.value (2).toByteArray());
        QByteArray _cacheout;

        if (_cache.get (_node.PFRN, _cacheout))
        {
            m_tmpQuery.bindValue (0, _node.FRN);
            m_tmpQuery.bindValue (1, _cacheout + "\\" + _node.FileName);
            m_tmpQuery.exec();
            m_tmpQuery.finish();
            continue;
        }

        assert (_nodeStack.empty());
        _nodeStack.push (_node);

        while (true)
        {
            _existPathQuery.bindValue (0, _node.FRN);
            _existPathQuery.exec();

            if (_existPathQuery.next())
            {
                _existPathQuery.finish();
                _nodeStack.pop();
                break;
            }
            else
            {
                _existPathQuery.finish();
                _findPathQuery.bindValue (0, _node.PFRN);
                _findPathQuery.exec();

                if (_findPathQuery.next())
                {
                    QByteArray tmppath =  _findPathQuery.value (0).toByteArray();
                    _findPathQuery.finish();

                    while (_nodeStack.size() > 1)
                    {
                        _node = _nodeStack.pop();
                        m_tmpQuery.bindValue (0, _node.FRN);
                        tmppath += ("\\" + _node.FileName);
                        m_tmpQuery.bindValue (1, tmppath);
                        m_tmpQuery.exec();
                        m_tmpQuery.finish();
                    }

                    _node = _nodeStack.pop();
                    _cache.put (_node.PFRN, tmppath);
                    m_tmpQuery.bindValue (0, _node.FRN);
                    tmppath += ("\\" + _node.FileName);
                    m_tmpQuery.bindValue (1, tmppath);
                    m_tmpQuery.exec();
                    m_tmpQuery.finish();
                    break;
                }
                else
                {
                    _findPathQuery.finish();
                    _findIndexQuery.bindValue (0, _node.PFRN);
                    _findIndexQuery.exec();

                    if (_findIndexQuery.next())
                    {
                        _node.FRN = _findIndexQuery.value (0).toULongLong();
                        _node.PFRN = _findIndexQuery.value (1).toULongLong();
                        _node.FileName = _findIndexQuery.value (2).toByteArray();
                        _nodeStack.push (_node);
                        _findIndexQuery.finish();
                    }
                    else
                    {
                        Log::w (QString ("Unknow pfrn for file : ").append (_node.FileName));
                        QSqlQuery deleteQuery = QSqlQuery (m_masterQuery);
                        deleteQuery.prepare (QString ("delete from Master_%1 where frn=?").arg (m_Path));

                        while (!_nodeStack.empty())
                        {
                            deleteQuery.bindValue (0, _nodeStack.pop().FRN);
                            deleteQuery.exec();
                        }

                        break;
                    }
                }
            }
        }
    }

    m_tmpQuery.exec (QString ("update Path_%1 set path='%1:\\' where frn=?").arg (m_Path));
    m_tmpQuery.bindValue (0, qulonglong (RootFile_pfrn));
    m_tmpQuery.exec ();
    _findPathQuery.clear();
    _findIndexQuery.clear();
    _existPathQuery.clear();
    m_masterQuery.clear();
    m_tmpQuery.clear();
    data_pathDB->beginwrite();
    move();
    data_pathDB->endwrite();
    Log::v (QString ("Generate Path for volume[%1:\\] successed.").arg (m_Path));
}
#pragma endregion PathGen
///:~