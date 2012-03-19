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
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:log.h
#ifndef LOG_H
#define LOG_H

#include "global.h"
#include "threadsync.h"
#include <QString>
#include <QTime>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <Windows.h>
#include <stdarg.h>
#include <string>

BINGO_BEGIN_NAMESPACE

class Log
{
public:
    static void enable()
    {
        m_enable = true;

        if (m_firstenable)
        {
            atexit (&Log::onQuit);
            m_firstenable = false;
        }
    }
    static void disable()
    {
        m_enable = false;
    }
    static void setlogfile (const std::wstring& logfile)
    {
        m_logfile = QString::fromStdWString (logfile);
    }
    static void setlogfile (const QString % logfile)
    {
        m_logfile = logfile;
    }
    static void v (const std::wstring& log)
    {
        verbStr (QString::fromStdWString (log));
    }
    static void w (const std::wstring& log)
    {
        warningStr (QString::fromStdWString (log));
    }
    static void e (const std::wstring& log)
    {
        errStr (QString::fromStdWString (log));
    }
    static void v (const std::string& log)
    {
        verbStr (QString::fromStdString (log));
    }
    static void w (const std::string& log)
    {
        warningStr (QString::fromStdString (log));
    }
    static void e (const std::string& log)
    {
        errStr (QString::fromStdString (log));
    }
    static void v (const QString& log)
    {
        verbStr (log);
    }
    static void w (const QString& log)
    {
        warningStr (log);
    }
    static void e (const QString& log)
    {
        errStr (log);
    }
    static void v (const char* format, ...)
    {
        char log[512] = {0};
        va_list args;
        va_start (args,  format);
        sprintf (log, format, args);
        va_end (args);
        verbStr (QString::fromStdString (log));
    }
    static void w (const char* format, ...)
    {
        char log[512] = {0};
        va_list args;
        va_start (args,  format);
        sprintf (log, format, args);
        va_end (args);
        warningStr (QString::fromStdString (log));
    }
    static void e (const char* format, ...)
    {
        char log[512] = {0};
        va_list args;
        va_start (args,  format);
        sprintf (log, format, args);
        va_end (args);
        errStr (QString::fromStdString (log));
    }
    static void v (const wchar_t* format, ...)
    {
        wchar_t log[512] = {0};
        va_list args;
        va_start (args,  format);
        _vswprintf (log, format, args);
        va_end (args);
        verbStr (QString::fromStdWString (log));
    }
    static void w (const wchar_t* format, ...)
    {
        wchar_t log[512] = {0};
        va_list args;
        va_start (args,  format);
        _vswprintf (log, format, args);
        va_end (args);
        warningStr (QString::fromStdWString (log));
    }
    static void e (const wchar_t* format, ...)
    {
        wchar_t log[512] = {0};
        va_list args;
        va_start (args,  format);
        _vswprintf (log, format, args);
        va_end (args);
        errStr (QString::fromStdWString (log));
    }
    static void TimerBegin()
    {
        m_timerBegin.start();
    }
    static void TimerEnd (const std::wstring& log)
    {
        timerStr (QString::fromStdWString (log));
    }
    static void TimerEnd (const std::string& log)
    {
        timerStr (QString::fromStdString (log));
    }
    static void TimerEnd (const QString& log)
    {
        timerStr (log);
    }
    static void TimerEnd (const char* format, ...)
    {
        char log[512] = {0};
        va_list args;
        va_start (args,  format);
        sprintf (log, format, args);
        va_end (args);
        timerStr (QString::fromStdString (log));
    }
    static void TimerEnd (const wchar_t* format, ...)
    {
        wchar_t log[512] = {0};
        va_list args;
        va_start (args,  format);
        _vswprintf (log, format, args);
        va_end (args);
        timerStr (QString::fromStdWString (log));
    }
private:
    static bool m_enable;
    static bool m_firstenable;
    static QString m_logfile;
    static Mutex m_logMutex;
    static QTime m_timerBegin;
    static void errStr (const QString& context)
    {
        logStr ("[error] " + context);
    }
    static void warningStr (const QString& context)
    {
        logStr ("[warning] " + context);
    }
    static void verbStr (const QString& context)
    {
        logStr ("[verbose] " + context);
    }
    static void timerStr (const QString& context)
    {
        QTime _timeLength = QTime (0, 0, 0).addMSecs (m_timerBegin.elapsed());
        logStr ("[timer] " + context + _timeLength.toString (" H:m:ss zzz"));
    }
    static void onQuit()
    {
        writelogfile ("===========================================================================");
    }
    static void logStr (const QString& context)
    {
        qDebug() << context;

        if (m_enable)
            writelogfile (QDateTime::currentDateTime().toString ("YYYY-MM-dd H:m:s z ") + context);
    }
    static void writelogfile (const QString& context)
    {
        synchronized (m_logMutex)
        {
            QFile file (m_logfile);

            if (file.open (QIODevice::Append | QIODevice::WriteOnly))
            {
                QTextStream stream (&file);
                stream << context << endl;
                stream.flush();
            }

            file.close();
        }
    }
};

bool Log::m_enable = false;
bool Log::m_firstenable = true;
QString Log::m_logfile = ".log";
Mutex Log::m_logMutex = Mutex();
QTime Log::m_timerBegin = QTime();

BINGO_END_NAMESPACE

#endif
///:~