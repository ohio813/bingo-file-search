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
///:multiprocessorthread.h
#ifndef MULTIPROCESSORTHREAD_H
#define MULTIPROCESSORTHREAD_H

#include "global.h"
#include "threadsync.h"
#include <Windows.h>

BINGO_FORWARD_DECLARE_CLASS(Mutex)
BINGO_BEGIN_NAMESPACE

class MultiProcessorThread
{
public:
    MultiProcessorThread();
    ~MultiProcessorThread();
    virtual void run();
    bool start();
    void suspend();
    void resume();
    void wait();
    DWORD wait (DWORD timeout);
    void terminate();
    HANDLE getHandle();
    DWORD_PTR getProcessorMask();
    static DWORD_PTR getRunningThreadCount();
    static DWORD_PTR getProcessorCount();
protected:
    static DWORD WINAPI ThreadFunc (LPVOID in);
    HANDLE m_hThread;
    static DWORD_PTR ProcessorCount;
    static DWORD_PTR RunningThreadCount;
    DWORD_PTR m_ProcessorMask;
    static DWORD_PTR GetNumCPUs();
    static Mutex RunningThreadCountMutex;
};

BINGO_END_NAMESPACE

#endif
///:~