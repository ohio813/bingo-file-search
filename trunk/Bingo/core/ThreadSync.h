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

#endif
///:~