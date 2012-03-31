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
///:stringconvert.h
#ifndef STRINGCONVERT_H
#define STRINGCONVERT_H

#include "global.h"
#include "globaldata.h"
#include <QByteArray>
#include <Windows.h>

BINGO_BEGIN_NAMESPACE

inline QByteArray Unicode2UTF (const wchar_t* buf, int len)
{
    int targetlen = WideCharToMultiByte (CP_UTF8, 0, buf, len, NULL, 0, NULL, NULL);
    char* utf8 = (char*) g_mempool.alloc (targetlen * sizeof (char));
    WideCharToMultiByte (CP_UTF8, 0, buf, len, &utf8[0], targetlen, NULL, NULL);
    QByteArray ret (utf8, targetlen);
    g_mempool.free (utf8);
    return std::move (ret);
}

__forceinline wchar_t ChartoWCharLetter (char Letter)
{
    return L'A' + (int) (Letter - 'A');
}

__forceinline char WChartoCharLetter (wchar_t Letter)
{
    return 'A' + (int) (Letter - L'A');
}

BINGO_END_NAMESPACE

#endif
///:~