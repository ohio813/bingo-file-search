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
///:Moniter.cpp
#include "Moniter.h"
#include "Data.h"
#include "NTFS.h"
#include "USN.h"
#include "File.h"
#include "VolumeLetter.h"
#pragma warning (disable : 4630)

extern void ReadLastUSNAddORUpdate (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                                    const DWORD& attrib)
{
    unsigned __int64 size = 0;
    FILETIME createTime, writeTime;
    bool fileInfoRet;

    if (attrib & FILE_ATTRIBUTE_DIRECTORY)
        fileInfoRet = File_Info_by_NTFS (ChartoWCharLetter (Path), frn , &createTime, &writeTime, NULL);
    else
        fileInfoRet = File_Info_by_NTFS (ChartoWCharLetter (Path), frn , &createTime, &writeTime, &size);

    if (fileInfoRet)
        data_masterDB->EnumInsert (Path, frn , pfrn, utf8name, attrib, FileSizeinKB (size),
                                   FILETIMEtoTIME32 (createTime), FILETIMEtoTIME32 (writeTime));
    else
        data_masterDB->EnumInsert (Path, frn , pfrn, utf8name, attrib, 0, 0, 0);
}
extern void ReadLastUSNDel (const char& Path, const unsigned __int64& frn)
{
    data_masterDB->ReadLastUSNDel (Path, frn);
}
extern void MoniterAdd (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                        const DWORD& attrib)
{
    data_Moniter->m_queue.put (MoniterNode (MTADD, Path, frn, pfrn, utf8name, attrib));
}
extern void MoniterDel (const char& Path, const unsigned __int64& frn)
{
    data_Moniter->m_queue.put (MoniterNode (MTDEL, Path, frn, 0, "", 0));
}
extern void MoniterUpdate (const char& Path, const unsigned __int64& frn, const unsigned __int64& pfrn, const std::string& utf8name,
                           const DWORD& attrib)
{
    if (attrib & FILE_ATTRIBUTE_DIRECTORY)
        data_Moniter->m_queue.put (MoniterNode (MTUPDATEDIR, Path, frn, pfrn, utf8name, attrib));
    else
        data_Moniter->m_queue.put (MoniterNode (MTUPDATE, Path, frn, pfrn, utf8name, attrib));
}

void Moniter::run()
{
    while (true)
    {
        MoniterNode node = m_queue.take();
        synchronized (m_mutex)
        {
        }
    }
}
void Moniter::pause()
{
    m_mutex.lock();
}
void Moniter::resume()
{
    m_mutex.unlock();
}
void Moniter::terminate()
{
    m_mutex.lock();
    QThread::terminate();
}
///:~