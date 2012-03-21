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
///:globaldata.cpp

#include "globaldata.h"
#include "log.h"
#include "../ntfs/ntfsdata.h"
#pragma warning (disable : 4630)

BINGO_BEGIN_NAMESPACE

MemoryPool g_mempool;

extern void initGlobalData()
{
    Log::v (L"initial global data.");
    initNTFSData();
}
extern void destroyGlobalData()
{
    Log::v (L"destroy global data.");
    destroyNTFSData();
}

BINGO_END_NAMESPACE

///:~