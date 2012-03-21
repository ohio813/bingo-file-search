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

BINGO_BEGIN_HEADER
BINGO_BEGIN_NAMESPACE

class B_LIB_EXPORT BingoTime
{
public:
    BingoTime();
    explicit BingoTime (const unsigned __int32 &time);
    static BingoTime fromFileTime (const FILETIME &fileTime);
    static BingoTime fromLocalFileTime (const FILETIME &localfileTime);
    static BingoTime fromSysTime (const SYSTEMTIME &sysTime);
    static BingoTime fromQDateTime (const QDateTime &qDTime);
    SYSTEMTIME toSysTime();
    FILETIME toFileTime();
    FILETIME toLocalFileTime();
    QDateTime toQDateTime();
    QString toString();
    unsigned __int32 toUInt32();
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

BINGO_END_NAMESPACE
BINGO_END_HEADER

#endif
///:~