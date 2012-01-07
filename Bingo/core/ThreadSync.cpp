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
///:ThreadSync.cpp

#include "ThreadSync.h"

Mutex::Mutex ()
{
    InitializeCriticalSection (&m_criticalSection);
}
Mutex::~Mutex ()
{
    DeleteCriticalSection (&m_criticalSection);
}
void Mutex::lock()
{
    EnterCriticalSection (&m_criticalSection);
}
bool Mutex::trylock()
{
    return TryEnterCriticalSection (&m_criticalSection);
}
void Mutex::unlock()
{
    LeaveCriticalSection (&m_criticalSection);
}

Lock::Lock (Mutex &mutex) : m_mutex (mutex), m_locked (true)
{
    m_mutex.lock();
}
Lock::~Lock ()
{
    m_mutex.unlock();
}
void Lock::setUnlock()
{
    m_locked = false;
}
Lock::operator bool() const
{
    return m_locked;
}

Semaphore::Semaphore (int initCount) : m_Value (initCount) {}
Semaphore::~Semaphore() {}
void Semaphore::wait (int count)
{
    m_Lock.lock();
    m_Value -= count;

    if (m_Value < 0)
    {
        HANDLE curThread = GetCurrentThread();
        HANDLE curProcess = GetCurrentProcess();
        DuplicateHandle (curProcess, curThread, curProcess, &curThread, 0, false, DUPLICATE_SAME_ACCESS);
        m_ThreadQueue.push (curThread);
        m_Lock.unlock();
        SuspendThread (curThread);
    }
    else
        m_Lock.unlock();
}
void Semaphore::signal (int count)
{
    m_Lock.lock();
    m_Value += count;

    if (m_Value <= 0)
    {
        HANDLE wakeupThread = m_ThreadQueue.front();
        m_ThreadQueue.pop();
        m_Lock.unlock();
        ResumeThread (wakeupThread);
    }
    else
        m_Lock.unlock();
}

ReadWriteSync::ReadWriteSync() : m_ReadCount (0) {}
ReadWriteSync::~ReadWriteSync() {}
void ReadWriteSync::beginread()
{
    synchronized (m_ReadCountLock)
    {
        m_ReadCount++;

        if (m_ReadCount == 1)
            m_WriteLock.lock();
    }
}
void ReadWriteSync::endread()
{
    synchronized (m_ReadCountLock)
    {
        m_ReadCount--;

        if (m_ReadCount == 0)
            m_WriteLock.unlock();
    }
}
void ReadWriteSync::beginwrite()
{
    m_WriteLock.lock();
}
void ReadWriteSync::endwrite()
{
    m_WriteLock.unlock();
}

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
///:~