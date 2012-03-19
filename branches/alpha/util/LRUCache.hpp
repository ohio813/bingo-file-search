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
///:LRUCache.hpp
#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP

#include <hash_map>
#include "../core/ThreadSync.h"

template<typename Key, typename Value>
class LRUCache
{
    struct  _MapNode
    {
        _MapNode (Value v,
                  Key former,
                  Key latter)
        {
            _v =  v;
            _former = former;
            _latter = latter;
        }
        Value _v;
        Key _former;
        Key _latter;
    };
public:
    LRUCache (int maxcap) : m_maxcap (maxcap)
    {
        m_data = std::hash_map<Key, _MapNode>();
    }
    ~LRUCache()
    {
        free();
    }
    inline void put (const Key &k, const Value &v)
    {
        synchronized (m_mutex)
        {
            if (m_data.size() == 0)
            {
                m_begin = k;
                m_end = k;
                _MapNode node (v, k, k);
                m_data.insert (std::pair<Key, _MapNode> (k, node));
            }
            else
            {
                std::pair<std::hash_map<Key, _MapNode>::iterator, bool> tryinsert = m_data.insert (std::pair<Key, _MapNode> (k, _MapNode (v, k, m_begin)));

                if (tryinsert.second)
                {
                    std::hash_map<Key, _MapNode>::iterator _oldbeginp = m_data.find (m_begin);
                    _MapNode newnode = _oldbeginp->second;
                    newnode._former = k;
                    _oldbeginp->second = newnode;
                    m_begin = k;

                    if (m_data.size() > m_maxcap)
                    {
                        std::hash_map<Key, _MapNode>::iterator _oldendp = m_data.find (m_end);
                        m_end = _oldendp->second._former;
                        m_data.erase (_oldendp);
                    }
                }
                else
                {
                    std::hash_map<Key, _MapNode>::iterator _p = tryinsert.first;

                    if (! (k == m_begin))
                    {
                        if (k == m_end)
                            m_end = _p->second._former;
                        else
                        {
                            std::hash_map<Key, _MapNode>::iterator _oldformer = m_data.find (_p->second._former);
                            std::hash_map<Key, _MapNode>::iterator _oldlatter = m_data.find (_p->second._latter);
                            _MapNode _oldformernode = _oldformer->second;
                            _MapNode _oldlatternode = _oldlatter->second;
                            _oldformernode._latter = _p->second._latter;
                            _oldlatternode._former = _p->second._former;
                            _oldformer->second = _oldformernode;
                            _oldlatter->second = _oldlatternode;
                        }

                        std::hash_map<Key, _MapNode>::iterator _oldbeginp = m_data.find (m_begin);
                        _MapNode _oldbeginnode = _oldbeginp->second;
                        _oldbeginnode._former = k;
                        _oldbeginp->second = _oldbeginnode;
                        _MapNode _newbeginnode = _p->second;
                        _newbeginnode._v = v;
                        _newbeginnode._latter = m_begin;
                        _p->second = _newbeginnode;
                        m_begin = k;
                    }
                }
            }
        }
    }
    inline bool get (const Key &k, Value &v)
    {
        bool m_Ret = false;
        synchronized (m_mutex)
        {
            std::hash_map<Key, _MapNode>::iterator _p;

            if ( (_p = m_data.find (k)) != m_data.end())
            {
                m_Ret = true;
                v = _p->second._v;

                if (! (k == m_begin))
                {
                    if (k == m_end)
                        m_end = _p->second._former;
                    else
                    {
                        std::hash_map<Key, _MapNode>::iterator _oldformer = m_data.find (_p->second._former);
                        std::hash_map<Key, _MapNode>::iterator _oldlatter = m_data.find (_p->second._latter);
                        _MapNode _oldformernode = _oldformer->second;
                        _MapNode _oldlatternode = _oldlatter->second;
                        _oldformernode._latter = _p->second._latter;
                        _oldlatternode._former = _p->second._former;
                        _oldformer->second = _oldformernode;
                        _oldlatter->second = _oldlatternode;
                    }

                    std::hash_map<Key, _MapNode>::iterator _oldbeginp = m_data.find (m_begin);
                    _MapNode _oldbeginnode = _oldbeginp->second;
                    _oldbeginnode._former = k;
                    _oldbeginp->second = _oldbeginnode;
                    _MapNode _newbeginnode = _p->second;
                    _newbeginnode._latter = m_begin;
                    _p->second = _newbeginnode;
                    m_begin = k;
                }
            }
        }
        return m_Ret;
    }
    inline void free()
    {
        synchronized (m_mutex)
        {
            m_data.clear();
        }
    }
private:
    int m_maxcap;
    Mutex m_mutex;
    std::hash_map<Key, _MapNode> m_data;
    Key m_begin;
    Key m_end;
};

#endif
///:~