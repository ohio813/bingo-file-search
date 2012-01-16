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
///:Log.h
#ifndef LOG_H
#define LOG_H

#include <string>
#include <QString>
#include <QTime>
#include "../core/ThreadSync.h"

class Log
{
public:
    static void enable();
    static void disable();
	static void onQuit();
    static void setlogfile (std::wstring logfile);
    static void setlogfile (QString logfile);
    static void v (std::wstring log);
    static void w (std::wstring log);
    static void e (std::wstring log);
    static void v (QString log);
    static void w (QString log);
    static void e (QString log);
    static void v (const char* log);
    static void w (const char* log);
    static void e (const char* log);
    static void v (wchar_t * format, ...);
    static void w (wchar_t * format, ...);
    static void e (wchar_t * format, ...);
    static void TimerBegin();
    static void TimerEnd (QString info);
    static void TimerEnd (std::wstring info);
    static void TimerEnd (const char* info);
    static void TimerEnd (wchar_t * format, ...);
private:
    static bool _enable;
	static bool _firstenable;
    static QString _logfile;
    static Mutex _logMutex;
	static QTime _timerBegin;
};

#endif
///:~