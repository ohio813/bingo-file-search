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
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:volhandlemgr.h
#ifndef VOLHANDLEMGR_H
#define VOLHANDLEMGR_H

#include "../core/global.h"
#include <Windows.h>
#include <wchar.h>

BINGO_BEGIN_NAMESPACE
class VolHandleMgr
{
public:
    VolHandleMgr();
    ~VolHandleMgr();
    HANDLE operator [] (char Letter);
    HANDLE operator [] (wchar_t Letter);
    bool open (char Letter);
    bool open (wchar_t Letter);
    void close (char Letter);
    void close (wchar_t Letter);
    bool isopen (char Letter);
    bool isopen (wchar_t Letter);
private:
    HANDLE m_hVols[26];
    bool _open (int i);
    void _close (int i);
};
BINGO_END_NAMESPACE
#endif
///:~