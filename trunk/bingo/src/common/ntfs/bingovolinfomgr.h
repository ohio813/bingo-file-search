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
*   Lesser General Public License for more details.'
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:bingovolinfomgr.h
#ifndef BINGOVOLINFOMGR_H
#define BINGOVOLINFOMGR_H

#include "../core/global.h"
#include <Windows.h>
#include <unordered_map>

BINGO_BEGIN_HEADER
BINGO_BEGIN_NAMESPACE

#pragma pack(push,1)
struct B_LIB_EXPORT BingoVolInfoNode
{
    wchar_t VolumeName[MAX_PATH];
    // volume unique guid name, in form of "\\?\Volume{GUID}\"
    wchar_t DeviceName[MAX_PATH];
    // an MS-DOS device name, such as: "\Device\HarddiskVolume1"
    wchar_t VolumeLabel[MAX_PATH];
    wchar_t FileSysType[10]; // file system such as: NTFS,FAT32,etc
    bool isLocalDisk: 2;
    bool isMounted: 2;
    bool isNTFS: 4;
    unsigned __int64 totalspace;
    unsigned __int64 freespace;
};
#pragma pack(pop)

class B_LIB_EXPORT BingoVolInfoMgr
{
public:
    BingoVolInfoMgr();
    void Scan();
    std::unordered_map<wchar_t, BingoVolInfoNode>& getAllInfos();
    BingoVolInfoNode getInfo (const char Path);
    BingoVolInfoNode getInfo (const wchar_t Path);
    int volNums();

private:
    std::unordered_map<wchar_t, BingoVolInfoNode> m_volinfos;
};

BINGO_END_NAMESPACE
BINGO_END_HEADER

#endif
///:~
