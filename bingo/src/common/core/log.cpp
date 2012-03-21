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
///:log.cpp

#include "global.h"
#include "log.h"
BINGO_BEGIN_NAMESPACE

bool Log::m_enable = false;
bool Log::m_firstenable = true;
QString Log::m_logfile = ".log";
Mutex Log::m_logMutex = Mutex();
QTime Log::m_timerBegin = QTime();

BINGO_END_NAMESPACE
///:~