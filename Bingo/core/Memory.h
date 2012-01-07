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
///:Memory.h
#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include "ThreadSync.h"
#include <Windows.h>
#include <string>
#include <list>
#include <assert.h>
#include <new>

void DumpBin (void* pSrc, size_t Size, std::wstring fileName);
void DumpTxt (void* pSrc, size_t Size, std::wstring fileName);
void DumpScreen (void* pSrc, size_t Size);

class HeapMgr
{
public:
    HeapMgr();
    ~HeapMgr();
    void* malloc (size_t Size);
    void* realloc (void* pSrc, size_t Size);
    void free (void* pSrc);
    bool isValidPointer (void* pSrc) ;
    void DumpBin (void* pSrc, std::wstring fileName);
    void DumpTxt (void* pSrc, std::wstring fileName);
    void DumpScreen (void* pSrc);
private:
    HANDLE m_hHeap;
};

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
    void* malloc (size_t Size);
    template<class T>
    T* mallocClass ();
    template<class T>
    T* mallocClassArray (int length);
    void* realloc (void* pSrc, size_t Size);
    void free (void* pSrc);
    template<class T>
    void freeClass (T *pSrc);
    template<class T>
    void freeClassArray (T *pSrc);
    bool isValidPointer (void* pSrc) ;
    void gc();
    void DumpPoolBin (std::wstring fileName);
    void DumpPoolTxt (std::wstring fileName);
    void DumpPoolScreen ();
private:
    void* mallocWithOutSync (size_t Size);
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
T* MemoryPool::mallocClass ()
{
    T* p = (T*) MemoryPool::malloc (sizeof (T));
    new (p) T;
    return p;
}

template<class T>
void MemoryPool::freeClass (T *pSrc)
{
    pSrc->~T();
    MemoryPool::free (pSrc);
}

template<class T>
T* MemoryPool::mallocClassArray (int length)
{
    assert (length > 0);
    unsigned char* p = (unsigned char*) MemoryPool::malloc (sizeof (T) * length + sizeof (int));
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
#endif
///:~