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

#pragma pack(push,1)
typedef struct VolInfoNode
{
    wchar_t VolumeName[MAX_PATH];
    // volume unique guid name, in form of "\\?\Volume{GUID}\"
    wchar_t DeviceName[MAX_PATH];
    // an MS-DOS device name, such as: "\Device\HarddiskVolume1"
    wchar_t VolumeLabel[MAX_PATH];
    wchar_t Path; // driver letter as 'C' for 'C:\'
    wchar_t FileSysType[10]; // file system such as: NTFS,FAT32,etc
    bool isLocalDisk: 2;
    bool isMounted: 2;
    bool isNTFS: 4;
    unsigned __int64 totalspace;
    unsigned __int64 freespace;
    bool operator < (const VolInfoNode n) const
    {
        int tmp = this->Path - n.Path;

        if (tmp == 0)
            return wcscmp (this->VolumeName, n.VolumeName) < 0 ;
        else
            return tmp < 0;
    }
} VolInfoNode;
#pragma pack(pop)

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