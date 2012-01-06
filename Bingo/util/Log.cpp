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
///:Log.cpp
#include "Log.h"
#include <stdarg.h>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <Windows.h>
#include <assert.h>
using namespace std;

bool Log::_enable = false;
QString Log::_logfile = ".log";
Mutex Log::_logMutex = Mutex();

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
    _logfile = QString::fromStdWString (logfile);
}
void Log::setlogfile (QString logfile)
{
    _logfile = logfile;
}
void Log::v (std::wstring log)
{
#ifdef _DEBUG
    log = L"[verbose]" + log + L"\n";
    OutputDebugStringW (log.c_str());
#else
    v (QString::fromStdWString (log));
#endif
}
void Log::w (std::wstring log)
{
#ifdef _DEBUG
    log = L"[warning]" + log + L"\n";
    OutputDebugStringW (log.c_str());
#else
    w (QString::fromStdWString (log));
#endif
}
void Log::e (std::wstring log)
{
#ifdef _DEBUG
    log = L"[error]" + log + L"\n";
    OutputDebugStringW (log.c_str());
    assert (false);
#else
    e (QString::fromStdWString (log));
#endif
}
void Log::v (QString log)
{
#ifdef _DEBUG
    v (log.toStdWString());
#else

    if (!_enable)
        return;

    QDateTime now = QDateTime::currentDateTime();
    synchronized (_logMutex)
    {
        QFile file (_logfile);

        if (file.open (QIODevice::Append | QIODevice::WriteOnly))
        {
            QTextStream stream (&file);
            stream << "verbose|" << now.toString() << "|" << log << endl;
            stream.flush();
        }

        file.close();
    }
#endif
}
void Log::w (QString log)
{
#ifdef _DEBUG
    w (log.toStdWString());
#else

    if (!_enable)
        return;

    QDateTime now = QDateTime::currentDateTime();
    synchronized (_logMutex)
    {
        QFile file (_logfile);

        if (file.open (QIODevice::Append | QIODevice::WriteOnly))
        {
            QTextStream stream (&file);
            stream << "warning|" << now.toString() << "|" << log << endl;
            stream.flush();
        }

        file.close();
    }
#endif
}
void Log::e (QString log)
{
#ifdef _DEBUG
    e (log.toStdWString());
#else

    if (!_enable)
        return;

    QDateTime now = QDateTime::currentDateTime();
    synchronized (_logMutex)
    {
        QFile file (_logfile);

        if (file.open (QIODevice::Append | QIODevice::WriteOnly))
        {
            QTextStream stream (&file);
            stream << "error|" << now.toString() << "|" << log << endl;
            stream.flush();
        }

        file.close();
    }
#endif
}
void Log::v (const char* log)
{
    v (QString (log));
}
void Log::w (const char* log)
{
    w (QString (log));
}
void Log::e (const char* log)
{
    e (QString (log));
}
void Log::v (wchar_t * format, ...)
{
    wchar_t log[512] = {0};
    va_list args;
    va_start (args, format);
    _vswprintf (log, format, args);
    va_end (args);
#ifdef _DEBUG
    v (wstring (log));
#else
    v (QString::fromStdWString (log));
#endif
}
void Log::w (wchar_t * format, ...)
{
    wchar_t log[512] = {0};
    va_list args;
    va_start (args,  format);
    _vswprintf (log, format, args);
    va_end (args);
#ifdef _DEBUG
    w (wstring (log));
#else
    w (QString::fromStdWString (log));
#endif
}
void Log::e (wchar_t * format, ...)
{
    wchar_t log[512] = {0};
    va_list args;
    va_start (args,  format);
    _vswprintf (log, format, args);
    va_end (args);
#ifdef _DEBUG
    e (wstring (log));
#else
    e (QString::fromStdWString (log));
#endif
}
///:~