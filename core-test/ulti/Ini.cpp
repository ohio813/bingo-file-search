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
///:Ini.cpp

#include "Ini.h"
#include "Log.h"
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
using namespace std;

#define _Ini_MaxSection 1024
#define _Ini_MaxAllSections 6000


bool File_Exist (wstring Filepath)
{
    return ::PathFileExistsW (Filepath.c_str()) && (!::PathIsDirectoryW (Filepath.c_str()));
}

vector<wstring> _Ini_SeprateBuff (LPWSTR lpszBuff)
{
    vector<wstring> List;
    List.clear();

    if (lpszBuff == NULL)
        return List;

    wchar_t *pchar = (wchar_t*) lpszBuff;
    int contiune_space (0);
    wstring temp (L"");

    for (; contiune_space < 2; ++pchar)
    {
        if (*pchar == L'\0')
        {
            contiune_space++;
            List.push_back (temp);
            temp = L"";
        }
        else
        {
            contiune_space = 0;
            temp += wstring (1, *pchar);
        }
    }

    List.pop_back();
    return List;
}
bool Ini_WriteSection (std::wstring Filepath, std::wstring Section, std::wstring SectionValue, bool Create)
{
    if (!File_Exist (Filepath))
        if (!Create)
        {
            SetLastError (ERROR_FILE_NOT_FOUND);
            Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
            return false;
        }

    return WritePrivateProfileSectionW ( (Section).c_str(), (SectionValue).c_str(), (Filepath).c_str());
}
bool Ini_WriteValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName, std::wstring Value, bool Create)
{
    if (!File_Exist (Filepath))
        if (!Create)
        {
            SetLastError (ERROR_FILE_NOT_FOUND);
            Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
            return false;
        }

    return WritePrivateProfileStringW ( (Section).c_str(), (ValueName).c_str(), (Value).c_str(), (Filepath).c_str());
}
bool Ini_WriteStructValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName, LPVOID Struct, UINT Size, bool Create)
{
    if (!File_Exist (Filepath))
        if (!Create)
        {
            SetLastError (ERROR_FILE_NOT_FOUND);
            Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
            return false;
        }

    return WritePrivateProfileStructW ( (Section).c_str(), (ValueName).c_str(), Struct, Size, (Filepath).c_str());
}
bool Ini_DeleteSection (std::wstring Filepath, std::wstring Section)
{
    if (File_Exist (Filepath))
        return WritePrivateProfileSectionW ( (Section).c_str(), NULL, (Filepath).c_str());
    else
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return false;
    }
}
bool Ini_DeleteValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName)
{
    if (File_Exist (Filepath))
        return WritePrivateProfileStringW ( (Section).c_str(), (ValueName).c_str(), NULL, (Filepath).c_str());
    else
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return false;
    }
}
bool Ini_ReadSectionNames (std::wstring Filepath, std::vector<std::wstring>&SectionNames)
{
    if (!File_Exist (Filepath))
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return false;
    }

    wchar_t lpszBuff[_Ini_MaxAllSections] = {0};
    GetPrivateProfileSectionNamesW (lpszBuff, _Ini_MaxAllSections, (Filepath).c_str());
    SectionNames = _Ini_SeprateBuff (lpszBuff);
    return true;
}
bool Ini_ReadValueNames (std::wstring Filepath, std::wstring Section, std::vector<std::wstring>&ValueNames)
{
    if (!File_Exist (Filepath))
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return false;
    }

    wchar_t lpszBuff[_Ini_MaxAllSections] = {0};
    GetPrivateProfileStringW ( (Section).c_str(), NULL, NULL, lpszBuff, _Ini_MaxAllSections, (Filepath).c_str());
    ValueNames = _Ini_SeprateBuff (lpszBuff);
    return true;
}
bool Ini_ReadSection (std::wstring Filepath, std::wstring Section, std::vector<std::wstring>&SectionValues)
{
    if (!File_Exist (Filepath))
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return false;
    }

    wchar_t lpszBuff[_Ini_MaxAllSections] = {0};
    GetPrivateProfileSectionW ( (Section).c_str(), lpszBuff, _Ini_MaxAllSections, (Filepath).c_str());
    SectionValues = _Ini_SeprateBuff (lpszBuff);
    return true;
}
std::wstring Ini_ReadValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName)
{
    std::wstring SectionValue;
    SectionValue.clear();

    if (!File_Exist (Filepath))
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return L"";
    }

    wchar_t out[_Ini_MaxSection];
    GetPrivateProfileStringW ( (Section).c_str(), (ValueName).c_str(), L"NoExist", out, _Ini_MaxSection, (Filepath).c_str());

    if (wstring (out) == L"NoExist")
    {
        GetPrivateProfileStringW ( (Section).c_str(), (ValueName).c_str(), L"Default", out, _Ini_MaxSection, (Filepath).c_str());

        if (wstring (out) == L"Default")
        {
            SetLastError (ERROR_INVALID_DATA);
            Log::e (L"ini read fails. no value[%s] or section[%s] exists in file %s .", ValueName.c_str(), Section.c_str(), Filepath.c_str());
        }

        return L"";
    }
    else
        SectionValue = wstring (out);

    return SectionValue;
}
bool Ini_ReadStructValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName, LPVOID Struct, UINT Size)
{
    if (!File_Exist (Filepath))
    {
        SetLastError (ERROR_FILE_NOT_FOUND);
        Log::e (L"ini operator fails. file[%s] not found.", Filepath.c_str());
        return false;
    }

    return GetPrivateProfileStructW ( (Section).c_str(), (ValueName).c_str(), Struct, Size, (Filepath).c_str());
}




///:~