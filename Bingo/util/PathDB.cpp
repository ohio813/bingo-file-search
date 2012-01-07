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
#include "Log.h"
#include "assert.h"

#pragma region PathDB
PathDB::PathDB()
{
    m_db = QSqlDatabase::addDatabase ("QSQLITE", "path_db");
    m_db.setDatabaseName (":memory:");
    m_db.open();
    m_query = QSqlQuery (m_db);
    m_query.exec ("PRAGMA encoding ='UTF-8';");
}
PathDB::~PathDB()
{
    m_db.close();
}
void PathDB::CreateTable (char Path)
{
    m_query.exec (QString ("create table Path_%1 "
                           "(frn integer(8) primary key, path vchar)").arg (Path));
    Log::v (QString ("Create table Path_%1").arg (Path));
}
void PathDB::DropTable (char Path)
{
    m_query.exec (QString ("delete from Path_%1").arg (Path));
    m_query.exec (QString ("drop table Path_%1").arg (Path));
    m_query.exec ("VACUUM;");
    Log::v (QString ("Drop table Path_%1").arg (Path));
}
QSqlQuery* PathDB::copyRootQuery()
{
    return new QSqlQuery (m_db);
}
extern bool SqlMemoryDBLoadorSave (QSqlDatabase& memdb, QSqlDatabase& filedb, bool load);
void PathDB::DumpDB()
{
    QSqlDatabase m_filedb = QSqlDatabase::addDatabase ("QSQLITE", "path_file_db");
    m_filedb.setDatabaseName (QCoreApplication::applicationDirPath() + "/Data/path.db");

    if (m_filedb.open())
        SqlMemoryDBLoadorSave (m_db, m_filedb, false);
}
#pragma endregion PathDB

#pragma region PathGen
void PathGen::setMasterQuery (QSqlQuery* masterQuery)
{
    m_masterQuery = masterQuery;
}
void PathGen::setPathQuery (QSqlQuery* pathQuery)
{
    m_pathQuery = pathQuery;
}
void PathGen::setPath (char path)
{
    m_Path = path;
}
void PathGen::run()
{
    m_pathQuery->prepare (QString ("insert into Path_%1 values (? ,'%1:')").arg (m_Path));
    m_pathQuery->bindValue (0, qulonglong (RootFile_pfrn));
    m_pathQuery->exec ();
    // pre-complie
    _findIndexQuery = QSqlQuery (*m_masterQuery);
    _findPathQuery = QSqlQuery (*m_pathQuery);
    _existPathQuery = QSqlQuery (*m_pathQuery);
	_findIndexQuery.setForwardOnly(true);
    _findIndexQuery.prepare (QString ("select frn,pfrn,name from Master_%1 where frn=?").arg (m_Path));
	_findPathQuery.setForwardOnly(true);
    _findPathQuery.prepare (QString ("select path from Path_%1 where frn=?").arg (m_Path));
	_existPathQuery.setForwardOnly(true);
    _existPathQuery.prepare (QString ("select 1 where exists(select 1 from Path_%1 where frn=?)").arg (m_Path));
    m_pathQuery->prepare (QString ("insert into Path_%1 values (?,?) ").arg (m_Path));
	m_masterQuery->setForwardOnly(true);
    m_masterQuery->exec (QString ("select frn,pfrn,name from Master_%1").arg (m_Path));

    while (m_masterQuery->next())
    {
        _Node _node (m_masterQuery->value (0).toULongLong(),
                     m_masterQuery->value (1).toULongLong(), m_masterQuery->value (2).toByteArray());
        QByteArray _cacheout;

        if (_cache.get (_node.PFRN, _cacheout))
        {
            m_pathQuery->bindValue (0, _node.FRN);
            m_pathQuery->bindValue (1, _cacheout + "\\" + _node.FileName);
            m_pathQuery->exec();
            m_pathQuery->finish();
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
                        m_pathQuery->bindValue (0, _node.FRN);
                        tmppath += ("\\" + _node.FileName);
                        m_pathQuery->bindValue (1, tmppath);
                        m_pathQuery->exec();
                        m_pathQuery->finish();
                    }

                    _node = _nodeStack.pop();
                    _cache.put (_node.PFRN, tmppath);
                    m_pathQuery->bindValue (0, _node.FRN);
                    tmppath += ("\\" + _node.FileName);
                    m_pathQuery->bindValue (1, tmppath);
                    m_pathQuery->exec();
                    m_pathQuery->finish();
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
                        QSqlQuery deleteQuery = QSqlQuery (*m_masterQuery);
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

    m_pathQuery->exec (QString ("update Path_%1 set path='%1:\\' where frn=?").arg (m_Path));
    m_pathQuery->bindValue (0, qulonglong (RootFile_pfrn));
    m_pathQuery->exec ();
    Log::v (QString ("Generate Path for volume[%1:\\] successed.").arg (m_Path));
}
#pragma endregion PathGen
///:~