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
///:Language.cpp
#include "Language.h"
#include "../util/Ini.h"
#include <string>
#include <vector>
using namespace std;

void Language::loadLang()
{
    wstring _defaultLang = Ini_ReadValue (L"lang/config.ini", L"Config", L"Default");
    QString defaultLang;

    if (_defaultLang == L"")
        defaultLang = "lang/en.qm";
    else
        defaultLang = QString::fromStdWString (L"lang/" + _defaultLang);

    QTranslator qtTranslator;
    qtTranslator.load (defaultLang);
    qApp->installTranslator (&qtTranslator);
}

QString Language::getCurLang()
{
    wstring _defaultLang = Ini_ReadValue (L"lang/config.ini", L"Config", L"Default");
    QString defaultLang;

    if (_defaultLang == L"")
        defaultLang = "en.qm";
    else
        defaultLang = QString::fromStdWString (_defaultLang);

    return defaultLang;
}

void Language::setCurLang (QString lang)
{
    QString defaultLang = "lang/" + lang;

    if (!QFile::exists (defaultLang))
        return;
    else
        Ini_WriteValue (L"lang/config.ini", L"Config", L"Default", lang.toStdWString());

    QTranslator qtTranslator;
    qtTranslator.load (defaultLang);
    qApp->installTranslator (&qtTranslator);
    emit refreshLangRequest();
}

map<QString, QString> Language::listAllLang()
{
    vector<wstring> SectionValues;
    map<QString, QString> ret;
    Ini_ReadSection (L"lang/config.ini", L"Lang", SectionValues);

    for (int i = 0; i < SectionValues.size(); ++i)
    {
        QString tmp = QString::fromStdWString (SectionValues[i]);
        int n = tmp.indexOf (QRegExp ("\\="));
        ret.insert (pair<QString, QString> (tmp.left (n), tmp.mid (n+1)));
    }

    return ret;
}
///:~