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
///:memory.h
#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include "global.h"
#include "threadsync.h"
#include <Windows.h>
#include <string>
#include <list>
#include <new>

BINGO_BEGIN_NAMESPACE

void DumpBin (void* pSrc, size_t Size, const std::wstring& fileName);
void DumpTxt (void* pSrc, size_t Size, const std::wstring& fileName);
void DumpScreen (void* pSrc, size_t Size);


class MemoryPool
{
    struct MemoryBlock
    {
        MemoryBlock()
        {
            this->Data = NULL;
            this->DataSize = 0;
            this->UsedSize = 0;
            this->IsAllocationBlock = false;
        }
        unsigned char *Data;
        size_t DataSize;
        size_t UsedSize;
        bool IsAllocationBlock;
    };
    typedef std::list<MemoryBlock>::iterator BlockPtr;
public:
    MemoryPool();
    ~MemoryPool();
    void* alloc (size_t Size);
    template<class T>
    T* allocClass ();
#ifdef _MSC_VER
    template<class T, class Arg0>
    T* allocClass (Arg0& arg0);
    template<class T, class Arg0, class Arg1>
    T* allocClass (Arg0& arg0, Arg1& arg1);
    template<class T, class Arg0, class Arg1, class Arg2>
    T* allocClass (Arg0& arg0, Arg1& arg1, Arg2& arg2);
#else// Currently not supported in VC but supported in GCC
    template<class T, class... Args>
    T* allocClass (Args && ... args);
#endif
    template<class T>
    T* allocClassArray (int length);
    void* realloc (void* pSrc, size_t Size);
    void free (void* pSrc);
    template<class T>
    void freeClass (T *pSrc);
    template<class T>
    void freeClassArray (T *pSrc);
    bool isValidPointer (void* pSrc) ;
    void gc();
    void DumpPoolBin (const std::wstring& fileName);
    void DumpPoolTxt (const std::wstring& fileName);
    void DumpPoolScreen ();
private:
    void* allocWithOutSync (size_t Size);
    bool AllocateMemory (const std::size_t &Size);
    void FreeAllAllocatedMemory();
    BlockPtr FindBlockSuitableToHoldMemory (const std::size_t &Size);
    BlockPtr FindBlockHoldingPointerTo (void* ptrMemoryBlock);
    void FreeBlocks (BlockPtr ptrBlock);
    void LinkBlocksToData (unsigned int BlockCount, unsigned char *ptrNewMemBlock);
private:
    Mutex m_allocMutex;
    std::list<MemoryBlock> m_memBlocks;
    BlockPtr m_pCurBlock;
    unsigned int m_objectCount;
    unsigned int m_blockCount;
    size_t m_totalPoolSize;
    size_t m_usedPoolSize;
    size_t m_freePoolSize;
};

template<class T>
T* MemoryPool::allocClass ()
{
    T* p = (T*) MemoryPool::alloc (sizeof (T));
    new (p) T;
    return p;
}

#ifdef _MSC_VER
template<class T, class Arg0>
T* MemoryPool::allocClass (Arg0& arg0)
{
    T* p = (T*) MemoryPool::alloc (sizeof (T));
    new (p) T (arg0);
    return p;
}
template<class T, class Arg0, class Arg1>
T* MemoryPool::allocClass (Arg0& arg0, Arg1& arg1)
{
    T* p = (T*) MemoryPool::alloc (sizeof (T));
    new (p) T (arg0, arg1);
    return p;
}
template<class T, class Arg0, class Arg1, class Arg2>
T* MemoryPool::allocClass (Arg0& arg0, Arg1& arg1, Arg2& arg2)
{
    T* p = (T*) MemoryPool::alloc (sizeof (T));
    new (p) T (arg0, arg1, arg2);
    return p;
}
#else// Currently not supported in VC but supported in GCC
template<class T, class... Args>
T* MemoryPool::allocClass (Args && ... args)
{
    T* p = (T*) MemoryPool::alloc (sizeof (T));
    new (p) T (std::forward<Args> (args)...);
    return p;
}
#endif

template<class T>
void MemoryPool::freeClass (T *pSrc)
{
    pSrc->~T();
    MemoryPool::free (pSrc);
}

template<class T>
T* MemoryPool::allocClassArray (int length)
{
    assert (length > 0);
    unsigned char* p = (unsigned char*) MemoryPool::alloc (sizeof (T) * length + sizeof (int));
    new (p) T[length];
    p += sizeof (int);
    return (T*) p;
}

template<class T>
void MemoryPool::freeClassArray (T *pSrc)
{
    unsigned char* p = (unsigned char*) pSrc;
    p -= sizeof (int);
    int length = * (int*) p;

    while (length)
        pSrc[--length].~T();

    MemoryPool::free (p);
}

BINGO_END_NAMESPACE

#endif
///:~