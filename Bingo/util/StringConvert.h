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
///:StringConvert.h
#ifndef STRINGCOVERNT_H
#define STRINGCOVERNT_H

#include <string>
#include <sstream>

namespace StringConvert
{
    template <class T>
    inline std::string num2string (const T &in)
    {
        static std::stringstream out;
        out.str ("");
        out.clear();
        out << in;
        return out.str();
    }
    template <class T>
    inline T string2num (const std::string &in)
    {
        T out;
        static std::stringstream tmp;
        tmp.str ("");
        tmp.clear();
        tmp << in;
        tmp >> out;
        return out;
    }
    template <class T>
    inline std::wstring num2wstring (const T &in)
    {
        static std::wstringstream out;
        out.str (L"");
        out.clear();
        out << in;
        return out.str();
    }
    template <class T>
    inline T wstring2num (const std::wstring &in)
    {
        T out;
        static std::wstringstream tmp;
        tmp.str (L"");
        tmp.clear();
        tmp << in;
        tmp >> out;
        return out;
    }
}

inline std::string _2string (const std::wstring &input);
inline std::string _utf2string (const std::string &utf8_input);
template<class T>
inline std::string _num2string (const T &num_input)
{
    return StringConvert::num2string<T> (num_input);
}

inline std::wstring _2wstring (const std::string &input);
inline std::wstring _utf2wstring (const std::string &utf8_input);
template<class T>
inline std::wstring _num2wstring (const T &num_input)
{
    return StringConvert::num2wstring<T> (num_input);
}

inline std::string _2utf8 (const std::wstring &input);
inline std::string _2utf8 (const std::string &input);
template<class T>
inline std::string _num2utf8 (const T &num_input)
{
    return _2utf8 (StringConvert::num2wstring<T> (num_input));
}

template<class T>
inline T _2num (const std::string &input)
{
    return StringConvert::string2num<T> (input);
}
template<class T>
inline T _2num (const std::wstring &input)
{
    return StringConvert::wstring2num<T> (input);
}
template<class T>
inline T _utf2num (const std::string &utf8_input)
{
    return StringConvert::wstring2num<T> (_utf2wstring (input));
}

#endif
///:~