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
    wstring defaultLang = Ini_ReadValue (L"lang/config.ini", L"Config", L"Default");
    QLocale defaultLocal;

    if (defaultLang == L"")
        defaultLocal = QLocale::system();
    else
        defaultLocal = QLocale (QString::fromStdWString (defaultLang));

    QTranslator qtTranslator;

    if (!qtTranslator.load (QString ("lang/bingo_%1.qm").arg (defaultLocal.name())))
        qtTranslator.load (QString ("lang/bingo_%1.qm").arg (QLocale (QLocale::English).name()));

    qApp->installTranslator (&qtTranslator);
}

QLocale Language::getCurLang()
{
    wstring defaultLang = Ini_ReadValue (L"lang/config.ini", L"Config", L"Default");

    if (defaultLang == L"")
    {
        QLocale locale = QLocale::system();

        if (QFile::exists (QString ("lang/bingo_%1.qm").arg (locale.name())))
            return locale;
        else
            return QLocale (QLocale::English);
    }
    else
        return QLocale (QString::fromStdWString (defaultLang));
}

void Language::setCurLang (QLocale lang)
{
    if (!QFile::exists (QString ("lang/bingo_%1.qm").arg (lang.name())))
        return;
    else
        Ini_WriteValue (L"lang/config.ini", L"Config", L"Default", lang.name().toStdWString());

    QTranslator qtTranslator;
    qtTranslator.load (QString ("lang/bingo_%1.qm").arg (lang.name()));
    qApp->installTranslator (&qtTranslator);
    emit refreshLangRequest();
}

vector<QLocale> Language::listAllLang()
{
    vector<wstring> SectionValues;
    vector<QLocale> ret;
    Ini_ReadSection (L"lang/config.ini", L"Lang", SectionValues);

    for (int i = 0; i < SectionValues.size(); ++i)
        ret.push_back (QLocale (QString::fromStdWString (SectionValues[i])));

    return ret;
}
///:~