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
#include "../util/Log.h"
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
            _tmpNode.Path = (_Path[0] <= L'Z' ? _Path[0] : _Path[0] - 32/*L'a'-L'A'*/);
            _tmpNode.isMounted = true;
        }
        else
        {
            _tmpNode.Path = 0;
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

        m_volinfos.insert (_tmpNode);
    }
    while (FindNextVolumeW (_vol_find_handle, _tmpNode.VolumeName, MAX_PATH * sizeof (wchar_t)));

    FindVolumeClose (_vol_find_handle);
    _vol_find_handle = INVALID_HANDLE_VALUE;
}

VolHandleMgr::VolHandleMgr()
{
    // set every m_hVol as INVALID_HANDLE_VALUE, since INVALID_HANDLE_VALUE = 0xffffffff
    memset (m_hVols, 0xff, 26 * 4);
}
VolHandleMgr::~VolHandleMgr()
{
    for (int i = 0; i < 26; ++i)
        _close (i);
}
HANDLE VolHandleMgr::operator [] (char Letter)
{
    return m_hVols[Letter - 'A'];
}
HANDLE VolHandleMgr::operator [] (wchar_t Letter)
{
    return m_hVols[Letter - L'A'];
}
bool VolHandleMgr::open (char Letter)
{
    return _open (Letter - 'A');
}
bool VolHandleMgr::open (wchar_t Letter)
{
    return _open (Letter - L'A');
}
void VolHandleMgr::close (char Letter)
{
    _close (Letter - 'A');
}
void VolHandleMgr::close (wchar_t Letter)
{
    _close (Letter - L'A');
}
bool VolHandleMgr::isopen (char Letter)
{
    return ! (INVALID_HANDLE_VALUE == m_hVols[Letter - 'A']);
}
bool VolHandleMgr::isopen (wchar_t Letter)
{
    return ! (INVALID_HANDLE_VALUE == m_hVols[Letter - L'A']);
}
bool VolHandleMgr::_open (int i)
{
    wchar_t _path[] = L"\\\\.\\X:";
    _path[4] = L'A' + i;
    m_hVols[i] = CreateFileW (_path, GENERIC_READ | GENERIC_WRITE,  FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING,  FILE_ATTRIBUTE_READONLY,  NULL);

    if (INVALID_HANDLE_VALUE == m_hVols[i])
    {
        Log::e (L"Get handle of driver[%c:\\] fails. error code:%d.", L'A' + i, ::GetLastError());
        return false;
    }
    else
    {
        Log::v (L"Get handle of driver[%c:\\] successes.", L'A' + i);
        return true;
    }
}
void VolHandleMgr::_close (int i)
{
    if (m_hVols[i] != INVALID_HANDLE_VALUE)
    {
        CloseHandle (m_hVols[i]);
        m_hVols[i] = INVALID_HANDLE_VALUE;
        Log::v (L"Close handle of driver[%c:\\].", L'A' + i);
    }
}
///:~