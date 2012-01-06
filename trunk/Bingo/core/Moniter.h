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
///:Moniter.h
#ifndef MONITER_H
#define MONITER_H
#include <QtCore>
#include <string>
#include <Windows.h>
#include "../util/BlockingQueue.hpp"

typedef void (*MoniterAddPtr) (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                               DWORD attrib);
typedef void (*MoniterDelPtr) (char Path, unsigned __int64 frn);
typedef void (*MoniterUpdatePtr) (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                                  DWORD attrib);

void ReadLastUSNAddORUpdate (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                             DWORD attrib);
void ReadLastUSNDel (char Path, unsigned __int64 frn);
void MoniterAdd (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                 DWORD attrib);
void MoniterDel (char Path, unsigned __int64 frn);
void MoniterUpdate (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                    DWORD attrib);

enum MoniterType {MTADD, MTDEL, MTUPDATE, MTUPDATEDIR};

typedef struct MoniterNode
{
    MoniterNode (MoniterType _type,
                 char _Path,
                 unsigned __int64 _frn,
                 unsigned __int64 _pfrn,
                 std::string _utf8name,
                 DWORD _attrib)
    {
        type = _type;
        Path = _Path;
        frn = _frn;
        utf8name = _utf8name;
        attrib = _attrib;
    }
    MoniterType type;
    char Path;
    unsigned __int64 frn;
    unsigned __int64 pfrn;
    std::string utf8name;
    DWORD attrib;
} MoniterNode;

class Moniter : public QThread
{
    Q_OBJECT
public:
    friend void MoniterAdd (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                            DWORD attrib);
    friend void MoniterDel (char Path, unsigned __int64 frn);
    friend void MoniterUpdate (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                               DWORD attrib);
private:
    BlockingQueue<MoniterNode> m_queue;
};

#endif
///:~