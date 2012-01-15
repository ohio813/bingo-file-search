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
///:StringConvert.cpp

#include "StringConvert.h"
//#include <vector>
#include <Windows.h>
#include "../core/Memory.h"
#include "../core/Data.h"
#pragma warning (disable : 4630)
using namespace std;

std::wstring ASCII2Unicode (const char* buf)
{
    int len = MultiByteToWideChar (CP_ACP, 0, buf, -1, NULL, 0);
    //std::vector<wchar_t> unicode (len);
    wchar_t *unicode = (wchar_t *) data_MemPool->malloc (len * sizeof (wchar_t));
    MultiByteToWideChar (CP_ACP, 0, buf, -1, &unicode[0], len);
    //return std::wstring (unicode.begin(), unicode.end());
    std::wstring ret (unicode, unicode + len * sizeof (wchar_t));
    data_MemPool->free (unicode);
    return std::move (ret);
}
std::string Unicode2ASCII (const wchar_t* buf)
{
    int len = WideCharToMultiByte (CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
    //std::vector<char> ascii (len);
    char* ascii = (char*) data_MemPool->malloc (len * sizeof (char));
    WideCharToMultiByte (CP_ACP, 0, buf, -1, &ascii[0], len, NULL, NULL);
    //return std::string (ascii.begin(), ascii.end());
    std::string ret (ascii, ascii + len * sizeof (char));
    data_MemPool->free (ascii);
    return std::move (ret);
}
std::wstring UTF2Unicode (const char* buf)
{
    int len = MultiByteToWideChar (CP_UTF8, 0, buf, -1, NULL, 0);
    //std::vector<wchar_t> unicode (len);
    wchar_t *unicode = (wchar_t *) data_MemPool->malloc (len * sizeof (wchar_t));
    MultiByteToWideChar (CP_UTF8, 0, buf, -1, &unicode[0], len);
    //return std::wstring (unicode.begin(), unicode.end());
    std::wstring ret (unicode, unicode + len * sizeof (wchar_t));
    data_MemPool->free (unicode);
    return std::move (ret);
}
std::string Unicode2UTF (const wchar_t* buf)
{
    int len = WideCharToMultiByte (CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL);
    //std::vector<char> utf8 (len);
    char* utf8 = (char*) data_MemPool->malloc (len * sizeof (char));
    WideCharToMultiByte (CP_UTF8, 0, buf, -1, &utf8[0], len, NULL, NULL);
    //return std::string (utf8.begin(), utf8.end());
    std::string ret (utf8, utf8 + len * sizeof (char));
    data_MemPool->free (utf8);
    return std::move (ret);
}

extern std::string _2string (const std::wstring &input)
{
    return Unicode2ASCII (input.c_str());
}
extern std::string _utf2string (const std::string &utf8_input)
{
    return Unicode2ASCII (UTF2Unicode (utf8_input.c_str()).c_str());
}

extern std::wstring _2wstring (const std::string &input)
{
    return ASCII2Unicode (input.c_str());
}
extern std::wstring _utf2wstring (const std::string &utf8_input)
{
    return UTF2Unicode (utf8_input.c_str());
}

extern std::string _2utf8 (const std::wstring &input)
{
    return Unicode2UTF (input.c_str());
}
extern std::string _2utf8 (const std::string &input)
{
    return Unicode2UTF (ASCII2Unicode (input.c_str()).c_str());
}


///:~