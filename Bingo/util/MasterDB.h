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
///:MasterDB.h
#ifndef MASTERDB_H
#define MASTERDB_H

#include <QtSql>
#include <string>
#include <Windows.h>
#include "../core/File.h"

class MasterDB
{
public:
    MasterDB();
    ~MasterDB();
    bool TableContains (char Path);
    void CreateTable (char Path);
    void DeleteTable (char Path);
    void DropTable (char Path);
    void EnumInsert (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                     DWORD attrib, DWORD size, TIME32 createTime, TIME32 writeTime);
    QSqlQuery* copyRootQuery();
private:
    QSqlDatabase m_db;
    QSqlDatabase m_filedb;
    QSqlQuery m_query;
};

#endif
///:~