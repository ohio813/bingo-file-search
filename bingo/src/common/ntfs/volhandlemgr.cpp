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
///:volhandlemgr.cpp

#include "volhandlemgr.h"
#include "../core/log.h"
BINGO_BEGIN_NAMESPACE

VolHandleMgr::VolHandleMgr()
{
    // set every m_hVol as INVALID_HANDLE_VALUE, since INVALID_HANDLE_VALUE = 0xffffffff
    memset (m_hVols, 0xff, 26 * 4);
}
VolHandleMgr::~VolHandleMgr()
{
    for (int i = 0; i < 26; ++i)
        _close (i);
}
HANDLE VolHandleMgr::operator [] (char Letter)
{
    return m_hVols[Letter - 'A'];
}
HANDLE VolHandleMgr::operator [] (wchar_t Letter)
{
    return m_hVols[Letter - L'A'];
}
bool VolHandleMgr::open (char Letter)
{
    return _open (Letter - 'A');
}
bool VolHandleMgr::open (wchar_t Letter)
{
    return _open (Letter - L'A');
}
void VolHandleMgr::close (char Letter)
{
    _close (Letter - 'A');
}
void VolHandleMgr::close (wchar_t Letter)
{
    _close (Letter - L'A');
}
bool VolHandleMgr::isopen (char Letter)
{
    return ! (INVALID_HANDLE_VALUE == m_hVols[Letter - 'A']);
}
bool VolHandleMgr::isopen (wchar_t Letter)
{
    return ! (INVALID_HANDLE_VALUE == m_hVols[Letter - L'A']);
}
bool VolHandleMgr::_open (int i)
{
    wchar_t _path[] = L"\\\\.\\X:";
    _path[4] = L'A' + i;
    m_hVols[i] = CreateFileW (_path, GENERIC_READ | GENERIC_WRITE,  FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING,  FILE_ATTRIBUTE_READONLY,  NULL);

    if (INVALID_HANDLE_VALUE == m_hVols[i])
    {
        Log::e (L"Get handle of driver[%c:\\] fails. error code:%d.", L'A' + i, ::GetLastError());
        return false;
    }
    else
    {
        Log::v (L"Get handle of driver[%c:\\] successes.", L'A' + i);
        return true;
    }
}
void VolHandleMgr::_close (int i)
{
    if (m_hVols[i] != INVALID_HANDLE_VALUE)
    {
        CloseHandle (m_hVols[i]);
        m_hVols[i] = INVALID_HANDLE_VALUE;
        Log::v (L"Close handle of driver[%c:\\].", L'A' + i);
    }
}

BINGO_END_NAMESPACE
///:~