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
///:Memory.cpp
#include "Memory.h"
#include "../util/Log.h"
#include <stdio.h>
#include <cmath>
#include <assert.h>
using namespace std;

void DumpBin (void* pSrc, size_t Size, std::wstring fileName)
{
    HANDLE hFile = CreateFileW (fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        Log::e (L"dump memory to bin fails,cannot open file: %s, error %d ", fileName.c_str(), GetLastError());
        return;
    }

    DWORD br;
    WriteFile (hFile, pSrc, Size, &br, NULL);
    CloseHandle (hFile);
}
void DumpTxt (void* pSrc, size_t Size, std::wstring fileName)
{
    unsigned char* _pSrc = (unsigned char*) pSrc;
    FILE* file = _wfopen (fileName.c_str(), L"a+");

    if (file == NULL)
    {
        Log::e (L"dump memory to txt fails,cannot open file: %s, error %d ", fileName.c_str(), GetLastError());
        return;
    }

    int i = 0, j;
    fprintf (file, "\n -- Mem Dump at:0x%016I64x -- \n", (__int64) pSrc);
    fprintf (file, "%8.8s ", " ");

    for (j = 0; j < 16; ++j) fprintf (file, " %2X", j);

    fprintf (file, "  ");

    for (j = 0; j < 16; ++j) fprintf (file, "%1X", j);

    fprintf (file, "\n");

    while (i < Size)
    {
        fprintf (file, "%08x ", i);

        for (j = 0; j < 16; ++j)
            if ( (i + j) < Size) fprintf (file, " %02x", (unsigned __int8) _pSrc[i + j]);
            else fprintf (file, "   ");

        fprintf (file, "  ");

        for (j = 0; j < 16; ++j)
            if ( (i + j) < Size) fprintf (file, "%c", isprint ( (unsigned __int8) _pSrc[i + j]) ?  _pSrc [i + j] : '.');
            else fprintf (file, " ");

        fprintf (file, "\n");
        i += 16;
    }

    fprintf (file, " -- DONE -- \n");
    fflush (file);
    fclose (file);
}
void DumpScreen (void* pSrc, size_t Size)
{
    unsigned char* _pSrc = (unsigned char*) pSrc;
    int i = 0, j;
    printf ("\n -- Mem Dump at:0x%016I64x -- \n", (__int64) pSrc);
    printf ("%8.8s ", " ");

    for (j = 0; j < 16; ++j) printf (" %2X", j);

    printf ("  ");

    for (j = 0; j < 16; ++j) printf ("%1X", j);

    printf ("\n");

    while (i < Size)
    {
        printf ("%08x ", i);

        for (j = 0; j < 16; ++j)
            if ( (i + j) < Size) printf (" %02x", (unsigned __int8) _pSrc[i + j]);
            else printf ("   ");

        printf ("  ");

        for (j = 0; j < 16; ++j)
            if ( (i + j) < Size) printf ("%c", isprint ( (unsigned __int8) _pSrc[i + j]) ?  _pSrc [i + j] : '.');
            else printf (" ");

        printf ("\n");
        i += 16;
    }

    printf (" -- DONE -- \n");
}

#pragma region HeapMgr
HeapMgr::HeapMgr()
{
    if (NULL == (m_hHeap =::HeapCreate (0, 0, 0)))
    {
        Log::e (L"create heap fails.error:%d", GetLastError());
    }
}
HeapMgr::~HeapMgr()
{
    if (!::HeapDestroy (m_hHeap))
        Log::w (L"destroy heap fails.error:%d", GetLastError());
}
void* HeapMgr::malloc (size_t Size)
{
    return ::HeapAlloc (m_hHeap, 0, Size);
}
void* HeapMgr::realloc (void* pSrc, size_t Size)
{
    return ::HeapReAlloc (m_hHeap, 0, pSrc, Size);
}
void HeapMgr::free (void* pSrc)
{
    ::HeapFree (m_hHeap, 0, pSrc);
}
bool HeapMgr::isValidPointer (void* pSrc)
{
    return ::HeapValidate (m_hHeap, 0, pSrc);
}
void HeapMgr::DumpBin (void* pSrc, std::wstring fileName)
{
    size_t Size =::HeapSize (m_hHeap, 0, pSrc);
    ::DumpBin (pSrc, Size, fileName);
}
void HeapMgr::DumpTxt (void* pSrc, std::wstring fileName)
{
    size_t Size =::HeapSize (m_hHeap, 0, pSrc);
    ::DumpTxt (pSrc, Size, fileName);
}
void HeapMgr::DumpScreen (void* pSrc)
{
    size_t Size =::HeapSize (m_hHeap, 0, pSrc);
    ::DumpScreen (pSrc, Size);
}
#pragma endregion HeapMgr

#pragma region MemoryPool
#define mp_InitPoolSize 1024  //Bytes
#define mp_SingleBlockSize 128  //Bytes
#define mp_MinAllocSize 512  //Bytes
#ifdef _DEBUG
#define mp_DefaultDataContext 0xCC
#endif

#define mp_MaxValue(A, B) (A > B ? A : B)
#define mp_NeedBlocks(Size) ( (size_t) ceil ( (double) Size / (double) mp_SingleBlockSize))
#define mp_NeedBlocksSize(Size) (mp_NeedBlocks (Size) * mp_SingleBlockSize)

MemoryPool::MemoryPool()
{
    m_memBlocks.clear();
    m_pCurBlock = m_memBlocks.begin();
    m_objectCount = 0;
    m_blockCount = 0;
    m_totalPoolSize = 0;
    m_usedPoolSize = 0;
    m_freePoolSize = 0;
    bool ret = AllocateMemory (mp_InitPoolSize);
    assert (ret && "System ran out of memory.");
}
MemoryPool::~MemoryPool()
{
    FreeAllAllocatedMemory();

    if (m_objectCount != 0)
        Log::w (L"You have not freed all allocated objects of MemoryPool:0x%016I64x ", (__int64) this);
}
void* MemoryPool::malloc (size_t Size)
{
    void* p;
    synchronized (m_allocMutex)
    {
        p = mallocWithOutSync (Size);
    }
    return p;
}
void* MemoryPool::mallocWithOutSync (size_t Size)
{
    size_t needBlocksSize = mp_NeedBlocksSize (Size);
    BlockPtr ptrBlock = m_memBlocks.end();

    while (ptrBlock == m_memBlocks.end())
    {
        ptrBlock = FindBlockSuitableToHoldMemory (needBlocksSize) ;

        if (ptrBlock == m_memBlocks.end())
        {
            needBlocksSize = mp_MaxValue (needBlocksSize, mp_MinAllocSize) ;

            if (!AllocateMemory (needBlocksSize))
                return NULL;
        }
    }

    m_usedPoolSize += needBlocksSize ;
    m_freePoolSize -= needBlocksSize ;
    m_objectCount++ ;
    ptrBlock->UsedSize = needBlocksSize;
    return ( (void *) ptrBlock->Data) ;
}
void* MemoryPool::realloc (void* pSrc, size_t Size)
{
    m_allocMutex.lock();
    BlockPtr ptrBlock = FindBlockHoldingPointerTo (pSrc);

    if (ptrBlock != m_memBlocks.end())
    {
        size_t oldSize = ptrBlock->UsedSize;
        unsigned int oldBlocks = mp_NeedBlocks (ptrBlock->UsedSize);
        unsigned int newBlocks = mp_NeedBlocks (Size);

        if (newBlocks <= oldBlocks)
        {
            ptrBlock->UsedSize = Size;
#ifdef _DEBUG

            if (newBlocks < oldBlocks)
            {
                for (unsigned int i = 0; i < newBlocks; ++i)
                    ptrBlock++;

                memset ( ( (void *) ptrBlock->Data), mp_DefaultDataContext, (oldBlocks - newBlocks) *mp_SingleBlockSize);
            }

#endif
            size_t DeltaSize = (oldBlocks - newBlocks) * mp_SingleBlockSize;
            m_freePoolSize += DeltaSize;
            m_usedPoolSize -= DeltaSize;
            m_allocMutex.unlock();
            return pSrc;
        }
        else
        {
            BlockPtr ptr = ptrBlock;

            for (unsigned int i = 0; i < oldBlocks; ++i)
                ptr++;

            bool isGood  = true;

            for (unsigned int i = 0; i < newBlocks - oldBlocks && isGood; ++i)
            {
                if (ptr->UsedSize == 0)
                    ptr++;
                else
                    isGood = false;
            }

            if (isGood)
            {
                ptrBlock->UsedSize = Size;
                size_t DeltaSize = (newBlocks - oldBlocks) * mp_SingleBlockSize;
                m_freePoolSize -= DeltaSize;
                m_usedPoolSize += DeltaSize;
                m_allocMutex.unlock();
                return pSrc;
            }
            else
            {
                // cannot realloc without moving the data
                void* pNewSrc = mallocWithOutSync (Size);

                if (pNewSrc != NULL)
                    memcpy (pNewSrc, pSrc, oldSize);

                FreeBlocks (ptrBlock);
                m_objectCount--;
                m_allocMutex.unlock();
                return pNewSrc;
            }
        }
    }
    else
    {
        Log::e (L"realloc memory fails, request pSrc[0x%016I64x] isn't in pool", pSrc);
        m_allocMutex.unlock();
        return NULL;
    }
}
void MemoryPool::free (void* pSrc)
{
    synchronized (m_allocMutex)
    {
        BlockPtr ptrBlock = FindBlockHoldingPointerTo (pSrc);

        if (ptrBlock != m_memBlocks.end())
            FreeBlocks (ptrBlock);
        else
            Log::e (L"Free memory fails, request pSrc[0x%016I64x] isn't in pool", pSrc);

        assert ( (m_objectCount > 0) && "ERROR : Request to delete more Memory then allocated.") ;
        m_objectCount--;
    }
}
bool MemoryPool::isValidPointer (void* pSrc)
{
    for (BlockPtr ptrTempBlock = m_memBlocks.begin();
            ptrTempBlock != m_memBlocks.end(); ptrTempBlock++)
    {
        if (ptrTempBlock->Data == ( (unsigned char *) pSrc))
            return true ;
    }

    return false ;
}
void MemoryPool::DumpPoolBin (std::wstring fileName)
{
    HANDLE hFile = CreateFileW (fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        Log::e (L"dump memory to bin fails,cannot open file: %s, error %d ", fileName.c_str(), GetLastError());
        return;
    }

    DWORD br;

    for (BlockPtr ptr = m_memBlocks.begin();
            ptr != m_memBlocks.end(); ++ptr)
        WriteFile (hFile, ptr->Data, mp_SingleBlockSize, &br, NULL);

    CloseHandle (hFile);
}
void MemoryPool::DumpPoolTxt (std::wstring fileName)
{
    BlockPtr ptr = m_memBlocks.begin();
    FILE * file = _wfopen (fileName.c_str(), L"a+");

    if (file == NULL)
    {
        Log::e (L"dump memory to txt fails,cannot open file: %s, error %d ", fileName.c_str(), GetLastError());
        return;
    }

    int i = 0, j, k = 0;
    fprintf (file, "\n -- Mem Pool[0x%016I64x] Dump -- \n", (__int64) this);
    fprintf (file, "%8.8s ", " ");

    for (j = 0; j < 16; ++j) fprintf (file, " %2X", j);

    fprintf (file, "  ");

    for (j = 0; j < 16; ++j) fprintf (file, "%1X", j);

    fprintf (file, "\n");
    unsigned char* pSrc = ptr->Data;

    while (true)
    {
        fprintf (file, "%08x ", i);

        for (j = 0; j < 16; ++j)
            fprintf (file, " %02x", (unsigned __int8) pSrc[k + j]);

        fprintf (file, "  ");

        for (j = 0; j < 16; ++j)
            fprintf (file, "%c", isprint ( (unsigned __int8) pSrc[k + j]) ? pSrc [k + j] : '.');

        fprintf (file, "\n");
        i += 16;
        k += 16;

        if (k >= mp_SingleBlockSize)
        {
            ptr++;

            if (ptr == m_memBlocks.end())
                break;

            pSrc = ptr->Data;
            k = 0;
        }
    }

    fprintf (file, " -- DONE -- \n");
    fflush (file);
    fclose (file);
}
void MemoryPool::DumpPoolScreen ()
{
    BlockPtr ptr = m_memBlocks.begin();
    int i = 0, j, k = 0;
    printf ("\n -- Mem Pool[0x%016I64x] Dump -- \n", (__int64) this);
    printf ("%8.8s ", " ");

    for (j = 0; j < 16; ++j) printf (" %2X", j);

    printf ("  ");

    for (j = 0; j < 16; ++j) printf ("%1X", j);

    printf ("\n");
    unsigned char* pSrc = ptr->Data;

    while (true)
    {
        printf ("%08x ", i);

        for (j = 0; j < 16; ++j)
            printf (" %02x", (unsigned __int8) pSrc[k + j]);

        printf ("  ");

        for (j = 0; j < 16; ++j)
            printf ("%c", isprint ( (unsigned __int8) pSrc[k + j]) ? (unsigned char) pSrc [k + j] : '.');

        printf ("\n");
        i += 16;
        k += 16;

        if (k >= mp_SingleBlockSize)
        {
            ptr++;

            if (ptr == m_memBlocks.end())
                break;

            pSrc = ptr->Data;
            k = 0;
        }
    }

    printf (" -- DONE -- \n");
}
bool MemoryPool::AllocateMemory (const std::size_t &Size)
{
    unsigned int neededBlocks = mp_NeedBlocks (Size);
    std::size_t neededBlocksSize = neededBlocks * mp_SingleBlockSize;
    unsigned char* ptrNewMem = new unsigned char[neededBlocksSize];

    if (!ptrNewMem)
    {
        Log::e (L"System ran out of memory.");
        return false;
    }

    m_totalPoolSize += neededBlocksSize;
    m_freePoolSize += neededBlocksSize;
    m_blockCount += neededBlocks;
#ifdef _DEBUG
    memset ( ( (void *) ptrNewMem), mp_DefaultDataContext, neededBlocksSize) ;
#endif
    LinkBlocksToData (neededBlocks, ptrNewMem);
    return true;
}
void MemoryPool::FreeAllAllocatedMemory()
{
    for (BlockPtr ptr = m_memBlocks.begin();
            ptr != m_memBlocks.end(); ++ptr)
    {
        if (ptr->IsAllocationBlock)
            delete [] ptr->Data;
    }
}
MemoryPool::BlockPtr MemoryPool::FindBlockSuitableToHoldMemory (const std::size_t &Size)
{
    for (unsigned int i = 0; i < m_blockCount;)
    {
        if (m_pCurBlock == m_memBlocks.end())
            m_pCurBlock = m_memBlocks.begin();

        if (m_pCurBlock->UsedSize == 0)
        {
            if (m_pCurBlock->DataSize >= Size)
                return m_pCurBlock;
            else
            {
                unsigned int BlocksToSkip = mp_NeedBlocks (m_pCurBlock->DataSize);

                for (unsigned int j = 0; j < BlocksToSkip; ++j)
                    m_pCurBlock++;

                i += BlocksToSkip;
            }
        }
        else
        {
            unsigned int BlocksToSkip = mp_NeedBlocks (m_pCurBlock->UsedSize);

            for (unsigned int j = 0; j < BlocksToSkip; ++j)
                m_pCurBlock++;

            i += BlocksToSkip;
        }
    }

    return m_memBlocks.end();
}
MemoryPool::BlockPtr MemoryPool::FindBlockHoldingPointerTo (void* ptrMemoryBlock)
{
    for (BlockPtr ptrTempBlock = m_memBlocks.begin();
            ptrTempBlock != m_memBlocks.end(); ptrTempBlock++)
    {
        if (ptrTempBlock->Data == (unsigned char*) ptrMemoryBlock)
            return ptrTempBlock ;
    }

    return m_memBlocks.end();
}
void MemoryPool::FreeBlocks (BlockPtr ptrBlock)
{
    size_t BlockSize = mp_NeedBlocksSize (ptrBlock->UsedSize);
#ifdef _DEBUG
    memset ( ( (void *) ptrBlock->Data), mp_DefaultDataContext, BlockSize);
#endif
    ptrBlock->UsedSize = 0;
    m_usedPoolSize -= BlockSize;
    m_freePoolSize += BlockSize;
}
void MemoryPool::LinkBlocksToData (unsigned int BlockCount, unsigned char *ptrNewMemBlock)
{
    size_t MemOffSet = 0 ;
    //set first block node
    MemoryBlock firstBlockNode;
    firstBlockNode.IsAllocationBlock = true;
    firstBlockNode.Data = &ptrNewMemBlock[MemOffSet];
    firstBlockNode.DataSize = BlockCount * mp_SingleBlockSize;
    MemOffSet += mp_SingleBlockSize;
    m_pCurBlock = m_memBlocks.insert (m_memBlocks.end(), firstBlockNode);//push_back

    //set other nodes
    for (unsigned int i = 1; i < BlockCount; ++i)
    {
        MemoryBlock blockNode;
        blockNode.Data = &ptrNewMemBlock[MemOffSet];
        blockNode.DataSize = firstBlockNode.DataSize - MemOffSet;
        m_memBlocks.push_back (blockNode);
        MemOffSet += mp_SingleBlockSize;
    }
}
#pragma endregion MemoryPool
///:~