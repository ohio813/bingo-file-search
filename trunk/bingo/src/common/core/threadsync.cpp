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
///:threadsync.cpp

#include "threadsync.h"
BINGO_BEGIN_NAMESPACE

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

BINGO_END_NAMESPACE
///:~