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

typedef void (*MoniterAddPtr) (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                               const DWORD& attrib);
typedef void (*MoniterDelPtr) (const char& Path, const unsigned __int64& frn);
typedef void (*MoniterUpdatePtr) (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                                  const DWORD& attrib);

inline void ReadLastUSNAddORUpdate (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                                    const DWORD& attrib);
inline void ReadLastUSNDel (const char& Path, const unsigned __int64& frn);
inline void MoniterAdd (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                        const DWORD& attrib);
inline void MoniterDel (const char& Path, const unsigned __int64& frn);
inline void MoniterUpdate (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                           const DWORD& attrib);

enum MoniterType {MTADD, MTDEL, MTUPDATE, MTUPDATEDIR};

typedef struct MoniterNode
{
    MoniterNode ()
    {
        type = MTADD;
        Path = 0;
        frn = 0;
        utf8name = "";
        attrib = 0;
    }
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
    void run();
    void pause();
    void resume();
    void terminate();
    friend void MoniterAdd (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                            const DWORD& attrib);
    friend void MoniterDel (const char& Path, const unsigned __int64& frn);
    friend void MoniterUpdate (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                               const DWORD& attrib);
private:
    BlockingQueue<MoniterNode> m_queue;
    Mutex m_mutex;
};

#endif
///:~