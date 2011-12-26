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
///:Ini.h
#ifndef INI_H
#define INI_H

#include <Windows.h>
#include <vector>
#include <string>

inline bool File_Exist (std::wstring Filepath);
bool Ini_WriteSection (std::wstring Filepath, std::wstring Section, std::wstring SectionValue, bool Create = true);
bool Ini_WriteValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName, std::wstring Value, bool Create = true);
bool Ini_WriteStructValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName, LPVOID Struct, UINT Size, bool Create = true);
bool Ini_DeleteSection (std::wstring Filepath, std::wstring Section);
bool Ini_DeleteValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName);
bool Ini_ReadSectionNames (std::wstring Filepath, std::vector<std::wstring>&SectionNames);
bool Ini_ReadValueNames (std::wstring Filepath, std::wstring Section, std::vector<std::wstring>&ValueNames);
bool Ini_ReadSection (std::wstring Filepath, std::wstring Section, std::vector<std::wstring>&SectionValues);
std::wstring Ini_ReadValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName);
bool Ini_ReadStructValue (std::wstring Filepath, std::wstring Section, std::wstring ValueName, LPVOID Struct, UINT Size);

#endif
///:~