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

DWORD_PTR MutilProcessorThread::ProcessorCount = GetNumCPUs();
DWORD_PTR MutilProcessorThread::RunningThreadCount = 0;
Mutex MutilProcessorThread::RunningThreadCountMutex = Mutex();
DWORD_PTR MutilProcessorThread::GetNumCPUs()
{
    SYSTEM_INFO m_si = {0};
    GetSystemInfo (&m_si);
    return (DWORD_PTR) m_si.dwNumberOfProcessors;
}
DWORD WINAPI MutilProcessorThread::ThreadFunc (LPVOID in)
{
    static_cast<MutilProcessorThread*> (in)->run();
    synchronized (RunningThreadCountMutex)
    {
        RunningThreadCount--;
    }
    return 0;
}
DWORD_PTR MutilProcessorThread::getRunningThreadCount()
{
    DWORD_PTR ret;
    synchronized (RunningThreadCountMutex)
    {
        ret = RunningThreadCount;
    }
    return ret;
}
DWORD_PTR MutilProcessorThread::getProcessorCount()
{
    return ProcessorCount;
}
MutilProcessorThread::MutilProcessorThread() : m_hThread (INVALID_HANDLE_VALUE), m_ProcessorMask (1) {}
MutilProcessorThread::~MutilProcessorThread()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        CloseHandle (m_hThread);
}
void MutilProcessorThread::run() {}
void MutilProcessorThread::start()
{
    m_hThread = CreateThread (NULL, 0, MutilProcessorThread::ThreadFunc, this, CREATE_SUSPENDED, NULL);

    if (m_hThread != INVALID_HANDLE_VALUE)
    {
        synchronized (RunningThreadCountMutex)
        {
            RunningThreadCount++;
            m_ProcessorMask = 1 << ( (RunningThreadCount - 1) % ProcessorCount);
        }
        SetThreadAffinityMask (m_hThread, m_ProcessorMask);
        ResumeThread (m_hThread);
    }
}
void MutilProcessorThread::suspend()
{
    SuspendThread (m_hThread);
}
void MutilProcessorThread::resume()
{
    ResumeThread (m_hThread);
}
void MutilProcessorThread::wait()
{
    WaitForSingleObject (m_hThread, INFINITE);
}
DWORD MutilProcessorThread::wait (DWORD timeout)
{
    return WaitForSingleObject (m_hThread, timeout);
}
void MutilProcessorThread::terminate()
{
    TerminateThread (m_hThread, 0);
    synchronized (RunningThreadCountMutex)
    {
        RunningThreadCount--;
    }
}
HANDLE MutilProcessorThread::getHandle()
{
    return m_hThread;
}
DWORD_PTR MutilProcessorThread::getProcessorMask()
{
    return m_ProcessorMask;
}
///:~