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
///:BlockingQueue.hpp
#ifndef BLOCKINGQUEUE_HPP
#define BLOCKINGQUEUE_HPP
#include "../core/ThreadSync.h"
#include <Windows.h>
#include <queue>

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue() {}
    ~BlockingQueue() {}
    inline bool empty()
    {
        synchronized (m_DataMutex)
        {
            return m_data.empty();
        }
    }
    inline size_t size()
    {
        synchronized (m_DataMutex)
        {
            return m_data.size();
        }
    }
    inline void put (const T &data)
    {
        synchronized (m_DataMutex)
        {
            m_data.push (data);
        }
        m_Semap.signal();
    }
    inline T take()
    {
        m_Semap.wait();
        T out;
        synchronized (m_DataMutex)
        {
            out = m_data.front();
            m_data.pop();
        }
        return std::move (out);
    }
private:
    std::queue<T> m_data;
    Mutex m_DataMutex;
    Semaphore m_Semap;
};

#endif
///:~