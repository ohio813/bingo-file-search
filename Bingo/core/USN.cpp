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
#include "VolumeLetter.h"
#include "NTFS.h"
#include "Moniter.h"
#include <string>
#include "../util/StringConvert.h"
#include "../util/Log.h"
using namespace std;

#define ENUM_BUF_LEN 32768 + sizeof(USN) //32K Buffer
#define READ_BUF_LEN 512 + sizeof(USN) //512B Buffer

bool VolUSN::StartUp()
{
    data_VolHandles->open (m_Path);
    m_hVol = (*data_VolHandles) [m_Path];
    data_pathDB->CreateTable (WChartoCharLetter (m_Path));
    bool jumpCreate = false;

    if (QueryUSN())
    {
        Log::v (L"Found exist USN journal of driver[%c:\\].", m_Path);
        jumpCreate = true;

        if (!data_masterDB->TableContains (m_Path))
            goto NewStart;

        QMap<char, ConfigDBLastRecordTableNode>::const_iterator ptr =
            data_configDB->m_LastRecord.find (WChartoCharLetter (m_Path));

        if (ptr == data_configDB->m_LastRecord.end())
        {
            data_masterDB->DeleteTable (m_Path);
            goto NewStart;
        }

        if (ptr.value().UsnJournalID != m_UsnInfo.UsnJournalID || ptr.value().NextUsn < m_UsnInfo.FirstUsn)
        {
            data_masterDB->DeleteTable (m_Path);
            goto NewStart;
        }

        m_readBuff = (char *) data_MemPool->malloc (READ_BUF_LEN);
        ReadUSN (ptr.value().NextUsn, false);
        Log::v (L"Read Last USN journal of driver[%c:\\] successes.", m_Path);
        m_hMonitor = CreateThread (NULL, 0, ReadUSNThread , this, 0, NULL);
    }
    else
    {
NewStart:
        data_masterDB->CreateTable (m_Path);
        Log::v (L"Start a new USN journal of driver[%c:\\].", m_Path);

        if (jumpCreate)
        {
            if (m_UsnInfo.FirstUsn == 0)
                goto JumpCreate;
            else
            {
                Log::v (L"Delete last time USN journal of driver[%c:\\].", m_Path);
                DeleteUSN();
            }
        }

        if (CreateUSN())
            Log::v (L"Create USN journal of driver[%c:\\] successes.", m_Path);
        else
        {
            Log::e (L"Create USN journal of driver[%c:\\] fails. error code:%d.", m_Path, ::GetLastError());
            return false;
        }

        if (QueryUSN())
            Log::v (L"Query USN journal of driver[%c:\\] successes.", m_Path);
        else
        {
            Log::e (L"Query USN journal of driver[%c:\\] fails. error code:%d.", m_Path, ::GetLastError());
            return false;
        }

JumpCreate:
        EnumUSN();
        Log::v (L"Enum USN journal of driver[%c:\\] successes.", m_Path);
        m_readBuff = (char *) data_MemPool->malloc (READ_BUF_LEN);
        m_hMonitor = CreateThread (NULL, 0, ReadUSNThread , this, 0, NULL);
    }

    return true;
}
void VolUSN::Exit()
{
    m_mutex.lock();
    TerminateThread (m_hMonitor, 0);
    data_MemPool->free (m_readBuff);
    data_configDB->m_LastRecord.insert (WChartoCharLetter (m_Path),
                                        ConfigDBLastRecordTableNode (m_UsnInfo.UsnJournalID, m_UsnInfo.NextUsn));
    data_pathDB->DropTable (WChartoCharLetter (m_Path));
    data_VolHandles->close (m_Path);
}
void VolUSN::Disable()
{
    m_mutex.lock();
    TerminateThread (m_hMonitor, 0);
    data_MemPool->free (m_readBuff);
    DeleteUSN();
    data_configDB->m_Disable.insert (WChartoCharLetter (m_Path));
    QMap<char, ConfigDBLastRecordTableNode>::iterator ptr =
        data_configDB->m_LastRecord.find (WChartoCharLetter (m_Path));

    if (ptr != data_configDB->m_LastRecord.end())
        data_configDB->m_LastRecord.erase (ptr);

    data_masterDB->DropTable (m_Path);
    data_pathDB->DropTable (WChartoCharLetter (m_Path));
    data_VolHandles->close (m_Path);
}
void VolUSN::PauseMonitor()
{
    m_mutex.lock();
}
void VolUSN::ResumeMonitor()
{
    m_mutex.unlock();
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
    char Path = WChartoCharLetter (m_Path);

    while (DeviceIoControl (m_hVol, FSCTL_ENUM_USN_DATA, &med,
                            sizeof (med), EnumBuff, ENUM_BUF_LEN, &usnDataSize, NULL) != 0)
    {
        DWORD dwRetBytes = usnDataSize - sizeof (USN);
        UsnRecord = (PUSN_RECORD) ( ( (char*) EnumBuff) + sizeof (USN));

        while (dwRetBytes > 0)
        {
            unsigned __int64 size = 0;
            FILETIME createTime, writeTime;
            bool fileInfoRet;

            if (UsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                fileInfoRet = File_Info_by_NTFS (m_Path, UsnRecord->FileReferenceNumber , &createTime, &writeTime, NULL);
            else
                fileInfoRet = File_Info_by_NTFS (m_Path, UsnRecord->FileReferenceNumber , &createTime, &writeTime, &size);

            if (fileInfoRet)
                data_masterDB->EnumInsert (Path, UsnRecord->FileReferenceNumber ,
                                           UsnRecord->ParentFileReferenceNumber, _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)),
                                           UsnRecord->FileAttributes, FileSizeinKB (size), FILETIMEtoTIME32 (createTime), FILETIMEtoTIME32 (writeTime));
            else
                data_masterDB->EnumInsert (Path, UsnRecord->FileReferenceNumber ,
                                           UsnRecord->ParentFileReferenceNumber, _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)),
                                           UsnRecord->FileAttributes, 0, 0, 0);

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
    m_mutex.lock();
    READ_USN_JOURNAL_DATA rujd;
    rujd.StartUsn = StartUsn;
    rujd.ReasonMask = 0xffffffff;
    rujd.ReturnOnlyOnClose = true;
    rujd.Timeout = 0;
    rujd.BytesToWaitFor = Monitor ? READ_BUF_LEN : 0;
    rujd.UsnJournalID = m_UsnInfo.UsnJournalID;
    DWORD usnDataSize;
    PUSN_RECORD UsnRecord;
    MoniterAddPtr _add = Monitor ? &MoniterAdd : &ReadLastUSNAddORUpdate;
    MoniterDelPtr _del = Monitor ? &MoniterDel : &ReadLastUSNDel;
    MoniterUpdatePtr _update = Monitor ? &MoniterUpdate : &ReadLastUSNAddORUpdate;
    char Path = WChartoCharLetter (m_Path);
    m_mutex.unlock();

    while (DeviceIoControl (m_hVol, FSCTL_READ_USN_JOURNAL, &rujd,
                            sizeof (rujd), m_readBuff, READ_BUF_LEN, &usnDataSize, NULL) != 0)
    {
        DWORD dwRetBytes = usnDataSize - sizeof (USN);
        UsnRecord = (PUSN_RECORD) ( ( (char*) m_readBuff) + sizeof (USN));

        while (dwRetBytes > 0)
        {
            m_mutex.lock();

            if (UsnRecord->Reason & USN_REASON_FILE_CREATE)
            {
                // File Created
                (*_add) (Path, UsnRecord->FileReferenceNumber, UsnRecord->ParentFileReferenceNumber,
                         _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)), UsnRecord->FileAttributes);
            }
            else if (UsnRecord->Reason & USN_REASON_FILE_DELETE)
            {
                // File Deleted
                (*_del) (Path, UsnRecord->FileReferenceNumber);
            }
            else
            {
                // File Modified
                (*_update) (Path, UsnRecord->FileReferenceNumber, UsnRecord->ParentFileReferenceNumber,
                            _2utf8 (wstring (UsnRecord->FileName, UsnRecord->FileNameLength / 2)), UsnRecord->FileAttributes);
                // Pay attention when changed reason it's folder's ParentFileReferenceNumber changed, which means this folder
                // has been moved to other place. In this case you need change every subfolder file's path in database.
            }

            m_mutex.unlock();
            // next record.
            DWORD recordLen = UsnRecord->RecordLength;
            dwRetBytes -= recordLen;
            UsnRecord = (PUSN_RECORD) ( ( (char*) UsnRecord) + recordLen);
        }

        rujd.StartUsn = * (USN *) m_readBuff;
        m_UsnInfo.NextUsn = rujd.StartUsn;

        if (!Monitor && dwRetBytes == 0)
            return;
    }
}

DWORD WINAPI ReadUSNThread (LPVOID param)
{
    VolUSN* volUSN = static_cast<VolUSN*> (param);
    volUSN->ReadUSN (volUSN->m_UsnInfo.NextUsn, true);
    return 0;
}
///:~