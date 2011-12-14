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
///:VolumeMgr.h
#ifndef VOLUMEMGR_H
#define VOLUMEMGR_H

#include <Windows.h>
#include <wchar.h>
#include <set>
#include <map>

typedef struct VolInfoNode
{
    wchar_t VolumeName[MAX_PATH];
    wchar_t DeviceName[MAX_PATH];
    bool isLocalDisk;
    bool isMounted;
    wchar_t Path[5];
    wchar_t VolumeLabel[MAX_PATH];
    bool isNTFS;
    wchar_t FileSysType[MAX_PATH];
    unsigned __int64 totalspace;
    unsigned __int64 freespace;
    bool operator < (const VolInfoNode n) const
    {
        int tmp = wcscmp (this->Path, n.Path);

        if (tmp == 0)
            return wcscmp (this->VolumeName, n.VolumeName) < 0 ;
        else
            return tmp < 0;
    }
} VolInfoNode;

class VolInfoMgr
{
public:
    VolInfoMgr();
    ~VolInfoMgr();
    void listVol();
    std::set<VolInfoNode, std::less<VolInfoNode>> m_volinfos;
};

class VolHandleMgr
{
public:
    VolHandleMgr();
    ~VolHandleMgr();
    HANDLE operator [] (char Letter);
    HANDLE operator [] (wchar_t Letter);
    bool open (char Letter);
    bool open (wchar_t Letter);
    void close (char Letter);
    void close (wchar_t Letter);
    bool isopen (char Letter);
    bool isopen (wchar_t Letter);
private:
    HANDLE m_hVols[26];
    bool _open (int i);
    void _close (int i);
};

#endif
///:~