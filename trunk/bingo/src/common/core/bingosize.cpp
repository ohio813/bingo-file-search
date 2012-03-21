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
///:bingosize.cpp

#include "bingosize.h"
BINGO_BEGIN_NAMESPACE

BingoSize::BingoSize()
{
    m_size = 0;
}
BingoSize::BingoSize (const unsigned __int32 &size)
{
    m_size = size;
}
BingoSize BingoSize::fromByte (const unsigned __int64 &size)
{
    unsigned __int32 bingosize = unsigned __int32 (size >> 10);

    if (size % 1024 != 0)
        bingosize++;

    return BingoSize (bingosize);
}
unsigned __int64 BingoSize::toByte()
{
    return ( (unsigned __int64) m_size) << 10;
}
QString BingoSize::toString()
{
    QString str;

    if (m_size < 1024)
        str.sprintf ("%d KB", m_size);
    else if (m_size < 1048576)
        str.sprintf ("%.2f MB", m_size / 1024.0);
    else
        str.sprintf ("%.2f GB", m_size / 1048576.0);

    return std::move (str);
}
unsigned __int32 BingoSize::toUInt32()
{
    return m_size;
}

#if !(defined BINGO_SHARED || defined BINGO_DLL)
#pragma warning (disable : 4630)
extern
#endif
QString VolSizeinStr (unsigned long long size)
{
    QString str;

    if (size <  1073741824)
        str.sprintf ("%d MB", size >> 20);
    else if (size < 1099511627776)
        str.sprintf ("%d GB", size >> 30);
    else
        str.sprintf ("%d TB", size >> 40);

    return std::move (str);
}

BINGO_END_NAMESPACE
///:~