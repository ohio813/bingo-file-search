/**
*	Copyright (C) 2011  Xu Cheng, Yang Zhengyu ,Zuo Zhiheng
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*	
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*	
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///:Log.cpp
#include "Log.h"
#include <stdarg.h>
#include <fstream>
#include <Windows.h>
using namespace std;

bool Log::_enable = false;
std::wstring Log::_logfile = L"";

void Log::enable()
{
    _enable = true;
}
void Log::disable()
{
    _enable = false;
}
void Log::setlogfile (std::wstring logfile)
{
    _logfile = logfile;
}
void Log::v (std::wstring log)
{
    if (!_enable)
        return;

    SYSTEMTIME curtime;
    ::GetLocalTime (&curtime);
    locale &loc = locale::global (locale (locale(), "", LC_CTYPE));
    wofstream ofile (_logfile, ios::app);
    locale::global (loc);
    ofile << L"verbose|" << curtime.wYear << L"-" << curtime.wMonth << L"-" << curtime.wDay << L" "
          << curtime.wHour << L":" << curtime.wMinute << L":" << curtime.wSecond << "|" << log << endl;
    ofile.close();
}
void Log::w (std::wstring log)
{
    if (!_enable)
        return;

    SYSTEMTIME curtime;
    ::GetLocalTime (&curtime);
    locale &loc = locale::global (locale (locale(), "", LC_CTYPE));
    wofstream ofile (_logfile, ios::app);
    locale::global (loc);
    ofile << L"warning|" << curtime.wYear << L"-" << curtime.wMonth << L"-" << curtime.wDay << L" "
          << curtime.wHour << L":" << curtime.wMinute << L":" << curtime.wSecond << "|" << log << endl;
    ofile.close();
}
void Log::e (std::wstring log)
{
    if (!_enable)
        return;

    SYSTEMTIME curtime;
    ::GetLocalTime (&curtime);
    locale &loc = locale::global (locale (locale(), "", LC_CTYPE));
    wofstream ofile (_logfile, ios::app);
    locale::global (loc);
    ofile << L"error|" << curtime.wYear << L"-" << curtime.wMonth << L"-" << curtime.wDay << L" "
          << curtime.wHour << L":" << curtime.wMinute << L":" << curtime.wSecond << "|" << log << endl;
    ofile.close();
}
void Log::v (wchar_t * format, ...)
{
    wchar_t log[512] = {0};
    va_list args;
    va_start (args, format);
    swprintf (log, format, args);
    va_end (args);
    v (wstring (log));
}
void Log::w (wchar_t * format, ...)
{
    wchar_t log[512] = {0};
    va_list args;
    va_start (args,  format);
    swprintf (log, format, args);
    va_end (args);
    w (wstring (log));
}
void Log::e (wchar_t * format, ...)
{
    wchar_t log[512] = {0};
    va_list args;
    va_start (args,  format);
    swprintf (log, format, args);
    va_end (args);
    e (wstring (log));
}
///:~