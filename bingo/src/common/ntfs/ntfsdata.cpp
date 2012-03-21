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
///:ntfsdata.cpp

#include "../core/global.h"
#include "ntfsdata.h"
#include "../core/log.h"
#pragma warning (disable : 4630)
BINGO_BEGIN_NAMESPACE

extern void initNTFSData()
{
    Log::v (L"initial ntfs data.");
}
extern void destroyNTFSData()
{
    Log::v (L"destory ntfs data.");
}

BINGO_END_NAMESPACE
///:~