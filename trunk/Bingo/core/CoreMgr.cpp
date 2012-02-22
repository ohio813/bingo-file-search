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
///:CoreMgr.cpp

#include "CoreMgr.h"
#include "VolumeMgr.h"
#include "Data.h"
#include "VolumeLetter.h"
#include "../util/ConfigDB.h"
#include "../util/MasterDB.h"
#include "../util/PathDB.h"
#include "../util/Log.h"
using namespace std;

CoreMgr::CoreMgr()
{
}

CoreMgr::~CoreMgr()
{
    foreach (VolUSN * volUSN, m_VolUsns)
    {
        volUSN->Exit();
        data_MemPool->freeClass (volUSN);
    }
}

void CoreMgr::run()
{
    emit appInitStart();
    Log::v (L"CoreMgr:app initial.");
    Log::TimerBegin();
    data_VolInfos = data_MemPool->mallocClass<VolInfoMgr>();
    data_configDB = data_MemPool->mallocClass<ConfigDB>();
    data_masterDB = data_MemPool->mallocClass<MasterDB>();
    data_pathDB = data_MemPool->mallocClass<PathDB>();
    data_Moniter = data_MemPool->mallocClass<Moniter>();
    QVector<PathGen*> pathGenVector;
    QSet<char> scanVolSet;
    double volCount = static_cast<double> (data_VolInfos->m_volinfos.size());
    double curVol = 0;

    for (std::set<VolInfoNode, std::less<VolInfoNode>>::iterator ptr = data_VolInfos->m_volinfos.begin()
            ; ptr != data_VolInfos->m_volinfos.end(); ++ptr)
    {
        if (ptr->isLocalDisk && ptr->isMounted && ptr->isNTFS)
        {
            char Path = WChartoCharLetter (ptr->Path);

            if (!data_configDB->m_Disable.contains (Path))
            {
                emit appInitProgress (static_cast<int> (curVol / volCount * 80), tr ("Scanning volume - %1:\\").arg (Path));
                VolUSN *volUSN = data_MemPool->mallocClass<VolUSN, const wchar_t> (ptr->Path);

                if (!volUSN->StartUp())
                    continue;

                m_VolUsns.insert (Path, volUSN);
                PathGen *pathGen = data_MemPool->mallocClass<PathGen>();
                pathGenVector.push_back (pathGen);
                pathGen->setPath (Path);
                scanVolSet.insert (Path);
            }
        }

        curVol++;
    }

    Log::v (L"CoreMgr:scan finished.");
	emit appInitProgress (80, tr ("Generate path."));
    data_MemPool->gc();
    foreach (QString tablename, data_masterDB->getAllTables())
    {
        char Path = tablename.at (tablename.length() - 1).toLatin1();

        if (!scanVolSet.contains (Path))
            data_masterDB->DropTable (Path);
    }
    Log::v (L"CoreMgr:clean disable or non-exist vol Table finished.");
    HANDLE *hPathGen = (HANDLE*) data_MemPool->malloc (pathGenVector.size() * sizeof (HANDLE));
    HANDLE *hPathGentmp = hPathGen;
    foreach (PathGen * pathGen, pathGenVector)
    {
        pathGen->start();
        *hPathGentmp++ = pathGen->getHandle();
    }
    Log::v (L"CoreMgr:PathGen started.");
    WaitForMultipleObjects (pathGenVector.size(), hPathGen, true, INFINITE);
    data_MemPool->free (hPathGen);
    foreach (PathGen * pathGen, pathGenVector) data_MemPool->freeClass (pathGen);
    data_MemPool->gc();
    Log::TimerEnd ("Boot Time:");
    emit appInitEnd();
}

///:~