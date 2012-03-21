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
///:bingotime.h
#ifndef BINGOTIME_H
#define BINGOTIME_H

#include "global.h"
#include <Windows.h>
#include <QString>
#include <QDateTime>

BINGO_BEGIN_NAMESPACE
BINGO_BEGIN_HEADER

class B_LIB_EXPORT BingoTime
{
public:
    BingoTime();
    explicit BingoTime (const unsigned __int32 &time);
    inline static BingoTime fromFileTime (const FILETIME &fileTime);
    inline static BingoTime fromLocalFileTime (const FILETIME &localfileTime);
    inline static BingoTime fromSysTime (const SYSTEMTIME &sysTime);
    inline static BingoTime fromQDateTime (const QDateTime &qDTime);
    inline SYSTEMTIME toSysTime();
    inline FILETIME toFileTime();
    inline FILETIME toLocalFileTime();
    inline QDateTime toQDateTime();
    inline QString toString();
    inline unsigned __int32 toUInt32();
private:
    unsigned __int32 m_time;
    enum {BINGOTIME_YEAR_START = 1970};
    /**
    * BingoTime definition:
    *   A 32-bit unsigned int to store how many
    *   minutes passed since 1970.1.1.
    *   It assumes that every month has 31 days.
    */
};

BINGO_END_HEADER
BINGO_END_NAMESPACE

#endif
///:~