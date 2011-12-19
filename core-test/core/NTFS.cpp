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
///:NTFS.cpp
#include <Windows.h>
#include <hash_map>
#include "NTFS.h"
#include "Data.h"
#include "Memory.h"
#include "..\util\Log.h"
#include <assert.h>
using namespace std;
using namespace NTFS;

namespace NTFS
{
    VolNTFSInfoCache *data_volNTFSInfoCache;

    VolNTFSInfoNode VolNTFSInfoCache::getInfoNode (wchar_t Path)
    {
        hash_map<wchar_t, VolNTFSInfoNode>::iterator ptr ;

        if ( (ptr = m_data.find (Path)) == m_data.end())
        {
            HANDLE hVol = (*data_VolHandles) [Path];
            NTFS_VOLUME_DATA_BUFFER ntfsVolData;
            DWORD br;
            VolNTFSInfoNode node;

            if (DeviceIoControl (hVol, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, &ntfsVolData,
                                 sizeof (NTFS_VOLUME_DATA_BUFFER), &br, NULL) == 0)
            {
                Log::e (L"Get ntfs volume data fails.driver:%c:\\, error:%d", Path, GetLastError());
                node.BytesPerFileRecordSegment = 1024;
                node.BytesPerCluster = 4096;
            }
            else
            {
                Log::v (L"Get ntfs volume data successes.driver:%c:\\", Path);
                node.BytesPerFileRecordSegment = ntfsVolData.BytesPerFileRecordSegment;
                node.BytesPerCluster = ntfsVolData.BytesPerCluster;
            }

            return m_data.insert (pair<wchar_t, VolNTFSInfoNode> (Path, node)).first->second;
        }
        else
            return ptr->second;
    }
    #pragma region NTFS-LCN
    __forceinline ULONG RunLength (PUCHAR run)
    {
        return (*run & 0x0f) + ( (*run >> 4) & 0x0f) + 1;
    }

    __forceinline LONGLONG RunLCN (PUCHAR run)
    {
        UCHAR n1 = *run & 0x0f;
        UCHAR n2 = (*run >> 4) & 0x0f;

        if (0 == n2) return 0;

        LONGLONG lcn = (CHAR) (run[n1 + n2]);
        LONG i = 0;

        for (i = n1 + n2 - 1; i > n1; --i)
            lcn = (lcn << 8) + run[i];

        return lcn;
    }

    __forceinline ULONGLONG RunCount (PUCHAR run)
    {
        UCHAR n =  *run & 0xf;
        ULONGLONG count = 0;
        ULONG i = 0;

        for (i = n; i > 0; i--)
            count = (count << 8) + run[i];

        return count;
    }
    #pragma endregion NTFS-LCN
}
//////////////////////////////////////////////////////////////////////////
bool File_Info_by_NTFS (wchar_t Path, DWORDLONG frn
                        , FILETIME *pOutTimeCreate
                        , FILETIME *pOutTimeLastWrite
                        , DWORDLONG *pOutSize)
{
    NTFS_FILE_RECORD_INPUT_BUFFER nfrib;
    nfrib.FileReferenceNumber.QuadPart = frn;
    VolNTFSInfoNode vnin = data_volNTFSInfoCache->getInfoNode (Path);
    char * Buff = (char*) data_MemPool->malloc (sizeof (NTFS_FILE_RECORD_OUTPUT_BUFFER) + vnin.BytesPerFileRecordSegment - 1);
    HANDLE hVol = (*data_VolHandles) [Path];
    DWORD br;

    if (DeviceIoControl (hVol, FSCTL_GET_NTFS_FILE_RECORD, &nfrib, sizeof (NTFS_FILE_RECORD_INPUT_BUFFER),
                         Buff, sizeof (NTFS_FILE_RECORD_OUTPUT_BUFFER) + vnin.BytesPerFileRecordSegment - 1, &br, NULL) == 0)
    {
        Log::e (L"Get NTFS file record fails. frn:0x%016I64x driver:%c:\\ error:%d", frn, Path, GetLastError());
        return false;
    }

    PFILE_RECORD_HEADER pfrh = (PFILE_RECORD_HEADER) ( (NTFS_FILE_RECORD_OUTPUT_BUFFER*) Buff)->FileRecordBuffer;
    PATTRIBUTE pa = (PATTRIBUTE) ( (char*) pfrh + pfrh->AttributesOffset);
    PSTANDARD_INFORMATION psi;

    while (pa->AttributeType != AttributeStandardInformation)
        pa = (PATTRIBUTE) ( (char*) pa + pa->Length);

    psi = PSTANDARD_INFORMATION ( (char*) pa + PRESIDENT_ATTRIBUTE (pa)->ValueOffset);

    if (pOutTimeLastWrite != NULL)
        memcpy (pOutTimeLastWrite, &psi->LastAccessTime, sizeof (FILETIME));

    if (pOutTimeCreate != NULL)
        memcpy (pOutTimeCreate, &psi->CreationTime, sizeof (FILETIME));

    if (pOutSize == NULL)
    {
        data_MemPool->free (Buff);
        return true; // for Dir.
    }

    while (pa->AttributeType != AttributeEnd && pa->AttributeType < AttributeData)
        pa = (PATTRIBUTE) ( (char*) pa + pa->Length);

    if (pa->AttributeType == AttributeEnd || pa->AttributeType > AttributeData)
    {
        // rescan to locate AttributeAttributeList
        for (pa = (PATTRIBUTE) ( (char*) pfrh + pfrh->AttributesOffset)
                  ; pa->AttributeType < AttributeAttributeList
                ; pa = (PATTRIBUTE) ( (char*) pa + pa->Length)
            );

        if (pa->AttributeType > AttributeAttributeList)
        {
            Log::w (L"unknown file size for frn:0x%016I64x driver:%c:\\, neither AttributeAttributeList nor AttributeData is found.", frn, Path);
            *pOutSize = 0;
            data_MemPool->free (Buff);
            return true;
        }

        if (pa->Nonresident)
        {
            PATTRIBUTE_LIST pal;
            PNONRESIDENT_ATTRIBUTE pna = PNONRESIDENT_ATTRIBUTE (pa);
            PBYTE pRun = (PBYTE) pa + pna->RunArrayOffset;
            ULONGLONG Lcn = RunLCN (pRun);
            ULONGLONG nCount = RunCount (pRun);
            assert (nCount <= pna->HighVcn - pna->LowVcn + 1);
            LARGE_INTEGER file_offset;
            file_offset.QuadPart = Lcn * vnin.BytesPerCluster;
            SetFilePointerEx (hVol, file_offset, NULL, FILE_BEGIN);
            PBYTE   pBuffferRead = (PBYTE) data_MemPool->malloc (vnin.BytesPerCluster);
            DWORD   dwRead = 0;
            ReadFile (hVol, pBuffferRead, vnin.BytesPerCluster, &dwRead, NULL);
            PBYTE   pBufferEnd = pBuffferRead + dwRead;

            for (pal = PATTRIBUTE_LIST (pBuffferRead);
                    pal->AttributeType != AttributeData;
                    pal = PATTRIBUTE_LIST (PBYTE (pal) + pal->Length)
                );

            if (pal->AttributeType == AttributeData)
            {
                nfrib.FileReferenceNumber.QuadPart = 0xffffffffffff & pal->FileReferenceNumber;

                if (DeviceIoControl (hVol, FSCTL_GET_NTFS_FILE_RECORD , &nfrib, sizeof (NTFS_FILE_RECORD_INPUT_BUFFER),
                                     Buff, sizeof (NTFS_FILE_RECORD_OUTPUT_BUFFER) + vnin.BytesPerFileRecordSegment - 1, &br, NULL) == 0)
                {
                    Log::w (L"Get NTFS file size record fails. frn:0x%016I64x driver:%c:\\ error:%d", frn, Path, GetLastError());
                    *pOutSize = 0;
                    data_MemPool->free (Buff);
                    data_MemPool->free (pBuffferRead);
                    return true;
                }

                pfrh = (PFILE_RECORD_HEADER) ( (NTFS_FILE_RECORD_OUTPUT_BUFFER*) Buff)->FileRecordBuffer;
                pa = (PATTRIBUTE) ( (char*) pfrh + pfrh->AttributesOffset);

                while (pa->AttributeType != AttributeData)
                    pa = (PATTRIBUTE) ( (char*) pa + pa->Length);

                if (pa->Nonresident)  *pOutSize = PNONRESIDENT_ATTRIBUTE (pa)->DataSize;
                else *pOutSize = PRESIDENT_ATTRIBUTE (pa)->ValueLength;
            }
            else
            {
                Log::w (L"unknown file size for frn:0x%016I64x driver:%c:\\,AttributeData in AttributeAttributeList(Nonresident) isn't found.", frn, Path);
                *pOutSize = 0;
                data_MemPool->free (Buff);
                data_MemPool->free (pBuffferRead);
                return true;
            }

            data_MemPool->free (pBuffferRead);
        }
        else
        {
            PATTRIBUTE_LIST pal;

            for (pal = PATTRIBUTE_LIST ( (PBYTE) pa + PRESIDENT_ATTRIBUTE (pa)->ValueOffset);
                    pal->AttributeType != AttributeEnd && pal->AttributeType < AttributeData;
                    pal = PATTRIBUTE_LIST (PBYTE (pal) + pal->Length)
                );

            if (pal->AttributeType == AttributeData)
            {
                nfrib.FileReferenceNumber.QuadPart = 0xffffffffffff & pal->FileReferenceNumber;

                if (DeviceIoControl (hVol, FSCTL_GET_NTFS_FILE_RECORD , &nfrib, sizeof (NTFS_FILE_RECORD_INPUT_BUFFER),
                                     Buff, sizeof (NTFS_FILE_RECORD_OUTPUT_BUFFER) + vnin.BytesPerFileRecordSegment - 1, &br, NULL) == 0)
                {
                    Log::w (L"Get NTFS file size record fails. frn:0x%016I64x driver:%c:\\ error:%d", frn, Path, GetLastError());
                    *pOutSize = 0;
                    data_MemPool->free (Buff);
                    return true;
                }

                pfrh = (PFILE_RECORD_HEADER) ( (NTFS_FILE_RECORD_OUTPUT_BUFFER*) Buff)->FileRecordBuffer;
                pa = (PATTRIBUTE) ( (char*) pfrh + pfrh->AttributesOffset);

                while (pa->AttributeType != AttributeData)
                    pa = (PATTRIBUTE) ( (char*) pa + pa->Length);

                if (pa->Nonresident) *pOutSize = PNONRESIDENT_ATTRIBUTE (pa)->DataSize;
                else *pOutSize = PRESIDENT_ATTRIBUTE (pa)->ValueLength;
            }
            else
            {
                Log::w (L"unknown file size for frn:0x%016I64x driver:%c:\\,AttributeData in AttributeAttributeList(Resident) isn't found.", frn, Path);
                *pOutSize = 0;
                data_MemPool->free (Buff);
                return true;
            }
        }
    }
    else
    {
        if (pa->Nonresident) *pOutSize = PNONRESIDENT_ATTRIBUTE (pa)->DataSize;
        else *pOutSize = PRESIDENT_ATTRIBUTE (pa)->ValueLength;
    }

    data_MemPool->free (Buff);
    return true;
}

///:~