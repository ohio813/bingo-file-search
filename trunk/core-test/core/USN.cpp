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
///:USN.cpp

#include "USN.h"
#include "Data.h"
#include "Memory.h"
#include <string>
#include "../util/StringConvert.h"
#include "../util/Log.h"
using namespace std;

#define ENUM_BUF_LEN 32768 + sizeof(USN) //32K Buffer
#define READ_BUF_LEN 512 + sizeof(USN) //512B Buffer

VolUSN::VolUSN (wchar_t Path)
{
    synchronized (m_isActive_Mutex)
    {
        m_isActive = false;
    }
    m_Path = Path;
    m_hVol = (*data_VolHandles) [m_Path];
}
void VolUSN::StartUp()
{
    if (QueryUSN())
    {
        Log::v (L"Found exist USN journal of driver[%c:\\].", m_Path);
        /*
        read old usn
        if db not exist goto NewStart
        if db.oldusn != m_UsnInfo.UsnJournalID (DeleteUSN();goto NewStart)
        ReadUSN(lastNextUSN,false)
        CreateThread
        */
    }
    else
    {
NewStart:
        Log::v (L"Start a new USN journal of driver[%c:\\].", m_Path);

        if (CreateUSN())
            Log::v (L"Create USN journal of driver[%c:\\] successes.", m_Path);
        else
        {
            Log::e (L"Create USN journal of driver[%c:\\] fails. error code:%d.", m_Path, ::GetLastError());
            synchronized (m_isActive_Mutex)
            {
                m_isActive = false;
            }
            return;
        }

        if (QueryUSN())
            Log::v (L"Query USN journal of driver[%c:\\] successes.", m_Path);
        else
        {
            Log::e (L"Query USN journal of driver[%c:\\] fails. error code:%d.", m_Path, ::GetLastError());
            synchronized (m_isActive_Mutex)
            {
                m_isActive = false;
            }
            return;
        }

        synchronized (m_isActive_Mutex)
        {
            m_isActive = true;
        }
        EnumUSN();
        m_hMonitor = CreateThread (NULL, 0, ReadUSNThread , this, 0, NULL);
    }
}
void VolUSN::Exit()
{
    synchronized (m_isActive_Mutex)
    {
        m_isActive = false;
    }
    WaitForSingleObject (m_hMonitor, 2000);
    //db flush
    // write m_UsnInfo.UsnJournalID m_UsnInfo.NextUsn
}
void VolUSN::Disable()
{
    synchronized (m_isActive_Mutex)
    {
        m_isActive = false;
    }
    WaitForSingleObject (m_hMonitor, 2000);
    DeleteUSN();
    // set db disable
}
bool VolUSN::isActive()
{
    synchronized (m_isActive_Mutex)
    {
        return m_isActive;
    }
}
bool VolUSN::CreateUSN()
{
    DWORD br;
    CREATE_USN_JOURNAL_DATA cujd;
    cujd.MaximumSize = 0; // default value
    cujd.AllocationDelta = 0; // default value
    return DeviceIoControl (m_hVol, FSCTL_CREATE_USN_JOURNAL, &cujd,
                            sizeof (cujd), NULL, 0, &br, NULL) != 0;
}
bool VolUSN::QueryUSN()
{
    DWORD br;
    return DeviceIoControl (m_hVol, FSCTL_QUERY_USN_JOURNAL, NULL,
                            0, &m_UsnInfo, sizeof (USN_JOURNAL_DATA), &br, NULL) != 0;
}
bool VolUSN::DeleteUSN()
{
    DWORD br;
    DELETE_USN_JOURNAL_DATA dujd;
    dujd.UsnJournalID = m_UsnInfo.UsnJournalID;
    dujd.DeleteFlags = USN_DELETE_FLAG_DELETE | USN_DELETE_FLAG_NOTIFY;
    return DeviceIoControl (m_hVol, FSCTL_DELETE_USN_JOURNAL, &dujd,
                            sizeof (dujd), NULL,  0, &br, NULL) != 0;
}
void VolUSN::EnumUSN()
{
    MFT_ENUM_DATA med;
    med.StartFileReferenceNumber = 0;
    med.LowUsn = m_UsnInfo.FirstUsn;
    med.HighUsn = m_UsnInfo.NextUsn;
    char* EnumBuff = (char *) data_MemPool->malloc (ENUM_BUF_LEN);
    DWORD usnDataSize;
    PUSN_RECORD UsnRecord;

    while (DeviceIoControl (m_hVol, FSCTL_ENUM_USN_DATA, &med,
                            sizeof (med), EnumBuff, ENUM_BUF_LEN, &usnDataSize, NULL) != 0)
    {
        DWORD dwRetBytes = usnDataSize - sizeof (USN);
        UsnRecord = (PUSN_RECORD) ( ( (char*) EnumBuff) + sizeof (USN));

        while (dwRetBytes > 0)
        {
            _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)); // Get file name coding in UTF-8.
            UsnRecord->FileReferenceNumber;
            UsnRecord->ParentFileReferenceNumber;
            UsnRecord->FileAttributes;
            // next record.
            DWORD recordLen = UsnRecord->RecordLength;
            dwRetBytes -= recordLen;
            UsnRecord = (PUSN_RECORD) ( ( (char*) UsnRecord) + recordLen);
        }

        med.StartFileReferenceNumber = * (USN *) EnumBuff;
    }

    data_MemPool->free (EnumBuff);
}
void VolUSN::ReadUSN (USN StartUsn, bool Monitor)
{
    READ_USN_JOURNAL_DATA rujd;
    rujd.StartUsn = StartUsn;
    rujd.ReasonMask = 0xffffffff;
    rujd.ReturnOnlyOnClose = true;
    rujd.Timeout = 0;
    rujd.BytesToWaitFor = 0;
    rujd.UsnJournalID = m_UsnInfo.UsnJournalID;
    char* ReadBuff = (char *) data_MemPool->malloc (READ_BUF_LEN);
    DWORD usnDataSize;
    PUSN_RECORD UsnRecord;

    while (DeviceIoControl (m_hVol, FSCTL_READ_USN_JOURNAL, &rujd,
                            sizeof (rujd), ReadBuff, READ_BUF_LEN, &usnDataSize, NULL) != 0)
    {
        DWORD dwRetBytes = usnDataSize - sizeof (USN);
        UsnRecord = (PUSN_RECORD) ( ( (char*) ReadBuff) + sizeof (USN));

        while (dwRetBytes > 0)
        {
            if (UsnRecord->Reason & USN_REASON_FILE_CREATE)
            {
                // File Created
                _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)); // Get file name coding in UTF-8.
                UsnRecord->FileReferenceNumber;
                UsnRecord->ParentFileReferenceNumber;
                UsnRecord->FileAttributes;
            }
            else if (UsnRecord->Reason & USN_REASON_FILE_DELETE)
            {
                // File Deleted
                UsnRecord->FileReferenceNumber;
            }
            else
            {
                // File Modified
                _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)); // Get file name coding in UTF-8.
                UsnRecord->FileReferenceNumber;
                UsnRecord->ParentFileReferenceNumber;
                UsnRecord->FileAttributes;
                // Pay attention when changed reason it's folder's ParentFileReferenceNumber changed, which means this folder
                // has been moved to other place. In this case you need change every subfolder file's path in database.
            }

            // next record.
            DWORD recordLen = UsnRecord->RecordLength;
            dwRetBytes -= recordLen;
            UsnRecord = (PUSN_RECORD) ( ( (char*) UsnRecord) + recordLen);
        }

        rujd.StartUsn = * (USN *) ReadBuff;
        m_UsnInfo.NextUsn = rujd.StartUsn;

        if (Monitor)
        {
            synchronized (m_isActive_Mutex)
            {
                if (!m_isActive)
                {
                    data_MemPool->free (ReadBuff);
                    return;
                }
            }
            Sleep (1500);
        }
        else
        {
            if (dwRetBytes == 0)
            {
                data_MemPool->free (ReadBuff);
                return;
            }
        }
    }

    data_MemPool->free (ReadBuff);
}

DWORD WINAPI ReadUSNThread (LPVOID param)
{
    VolUSN* volUSN = (VolUSN*) param;
    volUSN->ReadUSN (volUSN->m_UsnInfo.NextUsn, true);
    return 0;
}
///:~