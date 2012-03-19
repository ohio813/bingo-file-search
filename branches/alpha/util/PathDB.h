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
///:PathDB.h
#ifndef PATHDB_H
#define PATHDB_H

#include <QtSql>
#include <string>
#include <Windows.h>
#include "../core/ThreadSync.h"
#include "LRUCache.hpp"

class PathDB : public ReadWriteSync
{
public:
    PathDB();
    ~PathDB() {}
    void DropTable (char Path);
private:
    QSqlQuery m_query;
};

class PathGen : public MultiProcessorThread
{
    typedef struct _Node
    {
        _Node()
        {
            FRN = 0;
            PFRN = 0;
            FileName = "";
        }
        _Node (qulonglong frn, qulonglong pfrn, QByteArray filename)
        {
            FRN = frn;
            PFRN = pfrn;
            FileName = filename;
        }
        qulonglong FRN;
        qulonglong PFRN;
        QByteArray FileName;
    } _Node;
public:
    PathGen() : _cache (16) {}
    ~PathGen() {}
    void setPath (char path);
    void init();
    void move();
    void run();
private:
    QSqlDatabase m_tmpDB;
    QSqlQuery m_tmpQuery, m_masterQuery;
    char m_Path;
    QStack<_Node> _nodeStack;
    QSqlQuery _findPathQuery, _findIndexQuery, _existPathQuery;
    LRUCache<qulonglong, QByteArray> _cache;
};

#endif
///:~