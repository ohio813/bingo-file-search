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
///:File.h
#ifndef FILE_H
#define FILE_H
#include <string>
#include <Windows.h>

inline bool File_Exist (std::wstring Filepath);
inline bool Directory_Exist (std::wstring Directorypath);
inline bool Directory_Empty (std::wstring Directorypath);
BY_HANDLE_FILE_INFORMATION File_Info (std::wstring Filepath); // didn't check whether file exists
BY_HANDLE_FILE_INFORMATION File_Info_by_frn (unsigned __int64 frn, HANDLE hVol); // didn't check whether file exists

typedef unsigned __int32 TIME32;
#define TIME32_YEAR_START 1970
/**
* TIME32 definition:
*   A 32-bit unsigned int to store how many
*   minutes passed since 1970.1.1.
*   It assumes that every month has 31 days.
*/

#include <QDateTime>
#include <QDate>
#include <QTime>
__forceinline TIME32 SYSTIMEtoTIME32 (const SYSTEMTIME &sysTime);
__forceinline TIME32 QTIMEtoTIME32 (const QDateTime &qDTime);
__forceinline TIME32 FILETIMEtoTIME32 (const FILETIME &fileTime);
__forceinline void TIME32toSYSTIME (TIME32 time32, SYSTEMTIME &sysTime);

__forceinline unsigned __int32 FileSizeinKB (unsigned __int64 size)
{
    unsigned __int32 out = unsigned __int32 (size >> 10);

    if (size % 1024 != 0)
        out++;

    return out;
}
#endif
///:~