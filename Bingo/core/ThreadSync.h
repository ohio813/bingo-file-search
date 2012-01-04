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
///:ThreadSync.h
#ifndef THREADSYNC_H
#define THREADSYNC_H

#include <Windows.h>
#include <queue>

class Mutex
{
public:
    Mutex();
    ~Mutex();
    void lock();
    bool trylock();
    void unlock();
private:
    CRITICAL_SECTION m_criticalSection;
};

class Lock
{
public:
    Lock (Mutex &mutex);
    ~Lock ();
    void setUnlock();
    operator bool () const;
private:
    Mutex &m_mutex;
    bool m_locked;
};

#define synchronized(M)  for(Lock M##_lock = M; M##_lock; M##_lock.setUnlock())

class Semaphore
{
public:
    Semaphore (int initCount = 0);
    ~Semaphore();
    void wait (int count = 1);
    void signal (int count = 1);
private:
    int m_Value;
    Mutex m_Lock;
    std::queue<HANDLE> m_ThreadQueue;
};

class ReadWriteSync
{
public:
    ReadWriteSync();
    ~ReadWriteSync();
    void beginread();
    void endread();
    void beginwrite();
    void endwrite();
private:
    Mutex m_WriteLock;
    Mutex m_ReadCountLock;
    int m_ReadCount;
};

class MutilProcessorThread
{
public:
    MutilProcessorThread();
    ~MutilProcessorThread();
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

#endif
///:~