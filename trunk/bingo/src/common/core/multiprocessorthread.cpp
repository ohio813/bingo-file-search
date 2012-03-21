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
///:multiprocessorthread.cpp

#include "multiprocessorthread.h"
BINGO_BEGIN_NAMESPACE

DWORD_PTR MultiProcessorThread::ProcessorCount = GetNumCPUs();
DWORD_PTR MultiProcessorThread::RunningThreadCount = 0;
Mutex MultiProcessorThread::RunningThreadCountMutex = Mutex();
DWORD_PTR MultiProcessorThread::GetNumCPUs()
{
    SYSTEM_INFO m_si = {0};
    GetSystemInfo (&m_si);
    return (DWORD_PTR) m_si.dwNumberOfProcessors;
}
DWORD WINAPI MultiProcessorThread::ThreadFunc (LPVOID in)
{
    static_cast<MultiProcessorThread*> (in)->run();
    synchronized (RunningThreadCountMutex)
    {
        RunningThreadCount--;
    }
    return 0;
}
DWORD_PTR MultiProcessorThread::getRunningThreadCount()
{
    DWORD_PTR ret;
    synchronized (RunningThreadCountMutex)
    {
        ret = RunningThreadCount;
    }
    return ret;
}
DWORD_PTR MultiProcessorThread::getProcessorCount()
{
    return ProcessorCount;
}
MultiProcessorThread::MultiProcessorThread() : m_hThread (INVALID_HANDLE_VALUE), m_ProcessorMask (1) {}
MultiProcessorThread::~MultiProcessorThread()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        CloseHandle (m_hThread);
}
void MultiProcessorThread::run() {}
bool MultiProcessorThread::start()
{
    m_hThread = CreateThread (NULL, 0, MultiProcessorThread::ThreadFunc, this, CREATE_SUSPENDED, NULL);

    if (m_hThread != INVALID_HANDLE_VALUE)
    {
        synchronized (RunningThreadCountMutex)
        {
            RunningThreadCount++;
            m_ProcessorMask = 1 << ( (RunningThreadCount - 1) % ProcessorCount);
        }
        SetThreadAffinityMask (m_hThread, m_ProcessorMask);
        ResumeThread (m_hThread);
        return true;
    }
    else
        return false;
}
void MultiProcessorThread::suspend()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        SuspendThread (m_hThread);
}
void MultiProcessorThread::resume()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        ResumeThread (m_hThread);
}
void MultiProcessorThread::wait()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        WaitForSingleObject (m_hThread, INFINITE);
}
DWORD MultiProcessorThread::wait (DWORD timeout)
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        return WaitForSingleObject (m_hThread, timeout);
    else
        return WAIT_FAILED;
}
void MultiProcessorThread::terminate()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
    {
        TerminateThread (m_hThread, 1);
        DWORD exitCode;
        GetExitCodeThread (m_hThread, &exitCode);

        if (exitCode == STILL_ACTIVE)
        {
            WaitForSingleObject (m_hThread, 500);
            GetExitCodeThread (m_hThread, &exitCode);
        }

        if (exitCode == 1)
        {
            synchronized (RunningThreadCountMutex)
            {
                RunningThreadCount--;
            }
        }
    }
}
HANDLE MultiProcessorThread::getHandle()
{
    return m_hThread;
}
DWORD_PTR MultiProcessorThread::getProcessorMask()
{
    return m_ProcessorMask;
}

BINGO_END_NAMESPACE
///:~