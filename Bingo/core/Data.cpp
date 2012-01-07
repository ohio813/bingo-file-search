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
CoreMgr *data_coreMgr;
MemoryPool *data_MemPool;
VolInfoMgr *data_VolInfos;
VolHandleMgr *data_VolHandles;
ConfigDB *data_configDB;
MasterDB *data_masterDB;
PathDB *data_pathDB;
Moniter *data_Moniter;
Language *uidata_Lang;

void InitGlobalData()
{
    data_MemPool = new MemoryPool();
    uidata_Lang = data_MemPool->mallocClass<Language>();
    data_VolHandles = data_MemPool->mallocClass<VolHandleMgr>();
    NTFS::data_volNTFSInfoCache = data_MemPool->mallocClass<NTFS::VolNTFSInfoCache>();
    data_coreMgr = data_MemPool->mallocClass<CoreMgr>();
}
void DestroyGlobalData()
{
    data_MemPool->freeClass (data_coreMgr);
    data_MemPool->freeClass (data_Moniter);
    data_MemPool->freeClass (data_masterDB);
    data_MemPool->freeClass (data_pathDB);
    data_MemPool->freeClass (data_configDB);
    data_MemPool->freeClass (NTFS::data_volNTFSInfoCache);
    data_MemPool->freeClass (data_VolInfos);
    data_MemPool->freeClass (data_VolHandles);
    data_MemPool->freeClass (uidata_Lang);
    delete data_MemPool;
}

///:~