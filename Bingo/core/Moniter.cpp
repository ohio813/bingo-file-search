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


void ReadLastUSNAddORUpdate (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                             DWORD attrib)
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
void ReadLastUSNDel (char Path, unsigned __int64 frn)
{
    data_masterDB->ReadLastUSNDel (Path, frn);
}
void MoniterAdd (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                 DWORD attrib)
{
    data_Moniter->m_queue.put (MoniterNode (MTADD, Path, frn, pfrn, utf8name, attrib));
}
void MoniterDel (char Path, unsigned __int64 frn)
{
    data_Moniter->m_queue.put (MoniterNode (MTDEL, Path, frn, 0, "", 0));
}
void MoniterUpdate (char Path, unsigned __int64 frn, unsigned __int64 pfrn, std::string utf8name,
                    DWORD attrib)
{
    if (attrib & FILE_ATTRIBUTE_DIRECTORY)
        data_Moniter->m_queue.put (MoniterNode (MTUPDATEDIR, Path, frn, pfrn, utf8name, attrib));
    else
        data_Moniter->m_queue.put (MoniterNode (MTUPDATE, Path, frn, pfrn, utf8name, attrib));
}
///:~