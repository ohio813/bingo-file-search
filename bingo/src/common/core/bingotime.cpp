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
///:bingotime.cpp

#include "bingotime.h"
#include <QDate>
#include <QTime>
BINGO_BEGIN_NAMESPACE

BingoTime::BingoTime()
{
    this->m_time = 768;//1970-1-1 0:0
}
BingoTime::BingoTime (const unsigned __int32 &time)
{
    this->m_time = time;
}
BingoTime BingoTime::fromFileTime (const FILETIME &fileTime)
{
    FILETIME localFileTime;
    FileTimeToLocalFileTime (&fileTime, &localFileTime);
    SYSTEMTIME sysTime;
    FileTimeToSystemTime (&localFileTime, &sysTime);
    return fromSysTime (sysTime);
}
BingoTime BingoTime:: fromLocalFileTime (const FILETIME &localfileTime)
{
    SYSTEMTIME sysTime;
    FileTimeToSystemTime (&localfileTime, &sysTime);
    return fromSysTime (sysTime);
}
BingoTime BingoTime:: fromSysTime (const SYSTEMTIME &sysTime)
{
    unsigned __int32 time;
    time = (sysTime.wYear - BINGOTIME_YEAR_START) * 12;
    time = (time + sysTime.wMonth) * 31;
    time = (time + sysTime.wDay) * 24;
    time = (time + sysTime.wHour) * 60;
    time += sysTime.wMinute;
    return BingoTime (time);
}
BingoTime BingoTime::fromQDateTime (const QDateTime &qDTime)
{
    unsigned __int32 time;
    QDate qDate = qDTime.date();
    QTime qTime = qDTime.time();
    time = (qDate.year() - BINGOTIME_YEAR_START) * 12;
    time = (time + qDate.month()) * 31;
    time = (time + qDate.day()) * 24;
    time = (time + qTime.hour()) * 60;
    time += qTime.minute();
    return BingoTime (time);
}
SYSTEMTIME BingoTime::toSysTime()
{
    SYSTEMTIME sysTime;
    sysTime.wMinute = m_time % 60;
    m_time /= 60;
    sysTime.wHour = m_time % 24;
    m_time /= 24;
    sysTime.wDay = m_time % 31;

    if (sysTime.wDay == 0) sysTime.wDay = 31;

    m_time /= 31;
    sysTime.wMonth = m_time % 12;

    if (sysTime.wMonth == 0) sysTime.wMonth = 12;

    m_time /= 12;
    sysTime.wYear = m_time + BINGOTIME_YEAR_START;
    return sysTime;
}
FILETIME BingoTime::toFileTime()
{
    FILETIME localFileTime, fileTime;
    SystemTimeToFileTime (&toSysTime(), &localFileTime);
    LocalFileTimeToFileTime (&localFileTime, &fileTime);
    return fileTime;
}
FILETIME BingoTime::toLocalFileTime()
{
    FILETIME localFileTime;
    SystemTimeToFileTime (&toSysTime(), &localFileTime);
    return localFileTime;
}
QDateTime BingoTime::toQDateTime()
{
    SYSTEMTIME sysTime = toSysTime();
    return QDateTime (QDate (sysTime.wYear, sysTime.wMonth, sysTime.wDay), QTime (sysTime.wHour, sysTime.wMinute));
}
QString BingoTime::toString()
{
    SYSTEMTIME sysTime = toSysTime();
    QString str;
    str.sprintf ("%d-%02d-%02d %d:%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);
    return std::move (str);
}
unsigned __int32 BingoTime::toUInt32()
{
    return m_time;
}

BINGO_END_NAMESPACE
///:~