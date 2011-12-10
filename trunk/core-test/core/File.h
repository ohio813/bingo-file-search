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

bool File_Exist (std::wstring Filepath);
bool Directory_Exist (std::wstring Directorypath);
bool Directory_Empty (std::wstring Directorypath);
BY_HANDLE_FILE_INFORMATION File_Info (std::wstring Filepath); // didn't check whether file exists

typedef __int32 TIME32;
#define TIME32_YEAR_START 1980
/**
* TIME32 syntax:
*     year    month    day     hour    minute
*     0...0      0000    00000  00000   000000
*    12bits     4bits     5bits    5bits     6bits
*
* And year is started with TIME32_YEAR_START,
* in this case 1980.
**/

TIME32 SYSTIMEtoTIME32 (const SYSTEMTIME &sysTime);
void TIME32toSYSTIME (TIME32 time32, SYSTEMTIME &sysTime);
#endif
///:~