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
///:bingosize.h
#ifndef BINGOSIZE_H
#define BINGOSIZE_H

#include "global.h"
#include <QString>

BINGO_BEGIN_NAMESPACE
BINGO_BEGIN_HEADER

class B_LIB_EXPORT BingoSize
{
public:
    BingoSize();
    explicit BingoSize (const unsigned __int32 &size);
    inline static BingoSize fromByte (const unsigned __int64 &size);
    inline unsigned __int64 toByte();
    inline QString toString();
    inline unsigned __int32 toUInt32();
private:
    unsigned __int32 m_size;
    /**
    * BingoSize definition:
    *   A 32-bit unsigned int to store the
    *   file size in KB.
    */
};

#if defined BINGO_SHARED || defined BINGO_DLL
B_LIB_EXPORT QString VolSizeinStr (unsigned long long size);
#else
__forceinline QString VolSizeinStr (unsigned long long size);
#endif

BINGO_END_HEADER
BINGO_END_NAMESPACE

#endif
///:~