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

Semaphore::Semaphore() : m_Value (0) {}
Semaphore::Semaphore (long initCount) : m_Value (initCount) {}
Semaphore::~Semaphore() {}
void Semaphore::wait()
{
    m_Lock.lock();

    if (--m_Value < 0)
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
void Semaphore::signal (long count)
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

///:~