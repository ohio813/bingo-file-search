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
///:VolumeMgr.cpp

#include "VolumeMgr.h"
#include "../ulti/Log.h"
using namespace std;

VolInfoMgr::VolInfoMgr()
{
    m_volinfos = std::set<VolInfoNode, std::less<VolInfoNode>>();
    listVol();
}
VolInfoMgr::~VolInfoMgr()
{
    m_volinfos.clear();
}
void VolInfoMgr::listVol()
{
    m_volinfos.clear();
    VolInfoNode _tmpNode = {0};
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
            Log::e (L"Bad path name:[%s] for volume.", _tmpNode.VolumeName);
            continue;
        }

        _tmpNode.VolumeName[index] = L'\0';

        if (QueryDosDeviceW (&_tmpNode.VolumeName[4], _tmpNode.DeviceName, ARRAYSIZE (_tmpNode.DeviceName)) == 0)
            _tmpNode.DeviceName[0] = L'\0';

        _tmpNode.VolumeName[index] = L'\\';
        _tmpNode.isLocalDisk = (GetDriveTypeW (_tmpNode.VolumeName) == DRIVE_FIXED);
        DWORD CharCount = 5;

        if (GetVolumePathNamesForVolumeNameW (_tmpNode.VolumeName, _tmpNode.Path, CharCount, &CharCount)
                && CharCount > 1)
            _tmpNode.isMounted = true;
        else
        {
            _tmpNode.Path[0] = L'\0';
            _tmpNode.isMounted = false;
        }

        if (GetVolumeInformationW (_tmpNode.VolumeName, NULL, NULL, NULL, NULL, NULL, _tmpNode.FileSysType, MAX_PATH))
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

        m_volinfos.insert (_tmpNode);
    }
    while (FindNextVolumeW (_vol_find_handle, _tmpNode.VolumeName, MAX_PATH * sizeof (wchar_t)));

    FindVolumeClose (_vol_find_handle);
    _vol_find_handle = INVALID_HANDLE_VALUE;
}

///:~