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
#pragma warning (disable : 4630)
using namespace std;

extern bool File_Exist (std::wstring Filepath)
{
    return ::PathFileExistsW (Filepath.c_str()) && (!::PathIsDirectoryW (Filepath.c_str()));
}
extern bool Directory_Exist (std::wstring Directorypath)
{
    return ::PathIsDirectoryW (Directorypath.c_str());
}
extern bool Directory_Empty (std::wstring Directorypath)
{
    return ::PathIsDirectoryEmptyW (Directorypath.c_str());
}
BY_HANDLE_FILE_INFORMATION File_Info (std::wstring Filepath)
{
    HANDLE hFile = CreateFileW ( (Filepath).c_str(), GENERIC_READ, FILE_SHARE_READ,
                                 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
    BY_HANDLE_FILE_INFORMATION lpFileInformation;
    GetFileInformationByHandle (hFile, &lpFileInformation);
    CloseHandle (hFile);
    return lpFileInformation;
}
#pragma region File_Info_by_frn
/* Use Native API to get file handle straightly through frn, and get file information */
/* Ref: http://www.ragestorm.net/blogs/?p=186 */
typedef ULONG (__stdcall *pNtCreateFile) (
    PHANDLE FileHandle,
    ULONG DesiredAccess,
    PVOID ObjectAttributes,
    PVOID IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
);
typedef struct _UNICODE_STRING
{
    USHORT Length, MaximumLength;
    PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
    (p)->RootDirectory = r;                           \
    (p)->Attributes = a;                              \
    (p)->ObjectName = n;                              \
    (p)->SecurityDescriptor = s;                      \
    (p)->SecurityQualityOfService = NULL;             \
}

#define OBJ_CASE_INSENSITIVE 0x00000040L
#define FILE_NON_DIRECTORY_FILE 0x00000040
#define FILE_OPEN_BY_FILE_ID 0x00002000
#define FILE_OPEN 0x00000001
BY_HANDLE_FILE_INFORMATION File_Info_by_frn (unsigned __int64 frn, HANDLE hVol)
{
    static pNtCreateFile NtCreatefile = (pNtCreateFile) GetProcAddress (GetModuleHandle (L"ntdll.dll"), "NtCreateFile");
    HANDLE hFile;
    ULONG fid[2] = { (ULONG) (frn & 0xffffffff), (ULONG) ( (frn >> 32) & 0xffffffff) };
    UNICODE_STRING fidstr = {8, 8, (PWSTR) fid};
    OBJECT_ATTRIBUTES oa = {0};
    InitializeObjectAttributes (&oa, &fidstr, OBJ_CASE_INSENSITIVE, hVol, NULL);
    ULONG iosb[2];
    NtCreatefile (&hFile, GENERIC_ALL, &oa, iosb, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                  FILE_OPEN, FILE_OPEN_BY_FILE_ID , NULL, 0);
    BY_HANDLE_FILE_INFORMATION lpFileInformation;
    GetFileInformationByHandle (hFile, &lpFileInformation);
    CloseHandle (hFile);
    return lpFileInformation;
}
#pragma endregion

extern TIME32 SYSTIMEtoTIME32 (const SYSTEMTIME &sysTime)
{
    TIME32 time32;
    time32 = (sysTime.wYear - TIME32_YEAR_START) * 12;
    time32 = (time32 + sysTime.wMonth) * 31;
    time32 = (time32 + sysTime.wDay) * 24;
    time32 = (time32 + sysTime.wHour) * 60;
    time32 += sysTime.wMinute;
    return time32;
}
extern void TIME32toSYSTIME (TIME32 time32, SYSTEMTIME &sysTime)
{
    sysTime.wMinute = time32 % 60;
    time32 /= 60;
    sysTime.wHour = time32 % 24;
    time32 /= 24;
    sysTime.wDay = time32 % 31;

    if (sysTime.wDay == 0) sysTime.wDay = 31;

    time32 /= 31;
    sysTime.wMonth = time32 % 12;

    if (sysTime.wMonth == 0) sysTime.wMonth = 12;

    time32 /= 12;
    sysTime.wYear = time32 + TIME32_YEAR_START;
}
///:~
