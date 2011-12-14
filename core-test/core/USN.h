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
///:USN.h
#ifndef USN_H
#define USN_H

#include "ThreadSync.h"
#include <Windows.h>

#define RootFile_pfrn 0x0005000000000005  //the parent file reference number(pfrn) for Root File.

DWORD WINAPI ReadUSNThread (LPVOID param);

class VolUSN
{
public:
    VolUSN (wchar_t Path);
    ~VolUSN() {}
    void StartUp();
    void Exit();
    void Disable();
    bool isActive();
    friend DWORD WINAPI ReadUSNThread (LPVOID param);
private:
    bool CreateUSN();
    bool QueryUSN();
    bool DeleteUSN();
    void EnumUSN();
    void ReadUSN (USN StartUsn, bool Monitor);
private:
    wchar_t m_Path;
    HANDLE m_hVol;
    HANDLE m_hMonitor;
    USN_JOURNAL_DATA m_UsnInfo;
    bool m_isActive;
    Mutex m_isActive_Mutex;
};

#endif
///:~