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
///:File.cpp
#include "File.h"
#include <string>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
using namespace std;

inline bool File_Exist (std::wstring Filepath)
{
    return ::PathFileExistsW (Filepath.c_str()) && (!::PathIsDirectoryW (Filepath.c_str()));
}
inline bool Directory_Exist (std::wstring Directorypath)
{
    return ::PathIsDirectoryW (Directorypath.c_str());
}
inline bool Directory_Empty (std::wstring Directorypath)
{
    return ::PathIsDirectoryEmptyW (Directorypath.c_str());
}
inline BY_HANDLE_FILE_INFORMATION File_Info (std::wstring Filepath)
{
    HANDLE hFile = CreateFileW ( (Filepath).c_str(), GENERIC_READ, FILE_SHARE_READ,
                                 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    BY_HANDLE_FILE_INFORMATION lpFileInformation;
    GetFileInformationByHandle (hFile, &lpFileInformation);
    CloseHandle (hFile);
    return lpFileInformation;
}

__forceinline TIME32 SYSTIMEtoTIME32 (const SYSTEMTIME &sysTime)
{
    TIME32 time32;
    time32 = sysTime.wMinute;
    time32 |= (sysTime.wHour << 6);
    time32 |= (sysTime.wDay << 11);
    time32 |= (DWORD (sysTime.wMonth) << 16);
    time32 |= (DWORD (sysTime.wYear - TIME32_YEAR_START) << 20);
    return time32;
}
__forceinline void TIME32toSYSTIME (TIME32 time32, SYSTEMTIME &sysTime)
{
    sysTime.wMinute = time32 & 0x3f;
    time32 >>= 6;
    sysTime.wHour = time32 & 0x1f;
    time32 >>= 5;
    sysTime.wDay = time32 & 0x1f;
    time32 >>= 5;
    sysTime.wMonth = time32 & 0xf;
    time32 >>= 4;
    sysTime.wYear = time32 + TIME32_YEAR_START;
}
///:~
