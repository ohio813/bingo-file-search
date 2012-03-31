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
///:bingovolinfomgr.cpp

#include "../core/global.h"
#include "../core/log.h"
#include "../core/stringconvert.h"
#include "bingovolinfomgr.h"
using namespace std;

BINGO_BEGIN_NAMESPACE

BingoVolInfoMgr::BingoVolInfoMgr()
{
    Scan();
}

void BingoVolInfoMgr::Scan()
{
    m_volinfos.clear();
    BingoVolInfoNode _tmpNode = {0};
    wchar_t Path = 0;
    HANDLE _vol_find_handle = FindFirstVolumeW (_tmpNode.VolumeName, MAX_PATH * sizeof (wchar_t));

    if (_vol_find_handle == INVALID_HANDLE_VALUE)
    {
        Log::e (L"Finding volume failles. Error code:%i", ::GetLastError());
        return;
    }

    do
    {
        int index =  wcslen (_tmpNode.VolumeName) - 1;

        if (_tmpNode.VolumeName[0]     != L'\\' ||
                _tmpNode.VolumeName[1]     != L'\\' ||
                _tmpNode.VolumeName[2]     != L'?'  ||
                _tmpNode.VolumeName[3]     != L'\\' ||
                _tmpNode.VolumeName[index] != L'\\')
        {
            ::SetLastError (ERROR_BAD_PATHNAME);
            _tmpNode.VolumeName[index] = L'\0';
            Log::w (L"Bad path name:[%s] for volume.", _tmpNode.VolumeName);
            continue;
        }

        _tmpNode.VolumeName[index] = L'\0';

        if (QueryDosDeviceW (&_tmpNode.VolumeName[4], _tmpNode.DeviceName, ARRAYSIZE (_tmpNode.DeviceName)) == 0)
            _tmpNode.DeviceName[0] = L'\0';

        _tmpNode.VolumeName[index] = L'\\';
        _tmpNode.isLocalDisk = (GetDriveTypeW (_tmpNode.VolumeName) == DRIVE_FIXED);
        DWORD CharCount = 5;
        wchar_t _Path[5];

        if (GetVolumePathNamesForVolumeNameW (_tmpNode.VolumeName, _Path, CharCount, &CharCount)
                && CharCount > 1)
        {
            Path = (_Path[0] <= L'Z' ? _Path[0] : _Path[0] - 32/*L'a'-L'A'*/);
            _tmpNode.isMounted = true;
        }
        else
        {
            Path = 0;
            _tmpNode.isMounted = false;
        }

        if (GetVolumeInformationW (_tmpNode.VolumeName, _tmpNode.VolumeLabel, MAX_PATH, NULL, NULL, NULL, _tmpNode.FileSysType, 10))
            _tmpNode.isNTFS = (_wcsicmp (_tmpNode.FileSysType, L"NTFS") == 0);
        else
        {
            _tmpNode.isNTFS = false;
            _tmpNode.FileSysType[0] = L'\0';
        }

        ULARGE_INTEGER  _tspace, _fspace;

        if (GetDiskFreeSpaceExW (_tmpNode.VolumeName, NULL, &_tspace, &_fspace))
        {
            _tmpNode.totalspace = _tspace.QuadPart;
            _tmpNode.freespace = _fspace.QuadPart;
        }
        else
        {
            _tmpNode.totalspace = 0;
            _tmpNode.freespace = 0;
        }

        if (_tmpNode.isLocalDisk && _tmpNode.isMounted)
            m_volinfos.insert (pair<wchar_t, BingoVolInfoNode> (Path, _tmpNode));
    }
    while (FindNextVolumeW (_vol_find_handle, _tmpNode.VolumeName, MAX_PATH * sizeof (wchar_t)));

    FindVolumeClose (_vol_find_handle);
    _vol_find_handle = INVALID_HANDLE_VALUE;
}
std::unordered_map<wchar_t, BingoVolInfoNode>& BingoVolInfoMgr::getAllInfos()
{
    return m_volinfos;
}
BingoVolInfoNode BingoVolInfoMgr::getInfo (const char Path)
{
    return getInfo (ChartoWCharLetter (Path));
}
BingoVolInfoNode BingoVolInfoMgr::getInfo (const wchar_t Path)
{
    std::unordered_map<wchar_t, BingoVolInfoNode>::iterator ptr =
        m_volinfos.find (Path);

    if (ptr != m_volinfos.end())
        return ptr->second;
    else
    {
        Log::e (L"Try to access nonexistent volume:[%c:\\]", Path);
        BingoVolInfoNode nullNode;
        memset (&nullNode.VolumeName[0], 0, sizeof (BingoVolInfoNode));
        return nullNode;
    }
}
int BingoVolInfoMgr::volNums()
{
    return m_volinfos.size();
}

BINGO_END_NAMESPACE

///:~
