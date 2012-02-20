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
///:Data.cpp

#include "Data.h"
#include "NTFS.h"

// Store the global data.
MemoryPool *data_MemPool;
VolInfoMgr *data_VolInfos;
VolHandleMgr *data_VolHandles;

void InitGlobalData()
{
    data_MemPool = new MemoryPool();
    data_VolInfos = new VolInfoMgr();
    data_VolHandles = new VolHandleMgr();
    NTFS::data_volNTFSInfoCache = new NTFS::VolNTFSInfoCache();
}
void DestroyGlobalData()
{
    delete NTFS::data_volNTFSInfoCache;
    delete data_VolInfos;
    delete data_VolHandles;
    delete data_MemPool;
}

///:~