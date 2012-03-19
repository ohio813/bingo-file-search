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
///:blockingqueue.hpp
#ifndef BLOCKINGQUEUE_HPP
#define BLOCKINGQUEUE_HPP

#include "global.h"
#include "threadsync.h"
#include <Windows.h>
#include <queue>

BINGO_BEGIN_NAMESPACE

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue() {}
    ~BlockingQueue() {}
    inline bool empty()
    {
        bool ret;
        synchronized (m_DataMutex)
        {
            ret = m_data.empty();
        }
        return ret;
    }
    inline size_t size()
    {
        size_t ret;
        synchronized (m_DataMutex)
        {
            ret = m_data.size();
        }
        return ret;
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

BINGO_END_NAMESPACE

#endif
///:~