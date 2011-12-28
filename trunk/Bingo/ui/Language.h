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
///:Language.h
#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QtCore>
#include <QtGui>
#include <vector>

class Language: public QObject
{
    Q_OBJECT
public:
    void loadLang();
    QLocale getCurLang();
    std::vector<QLocale> listAllLang();
    void setCurLang (QLocale lang);
signals:
    void explains();
    void refreshLangRequest();
};

#endif
///:~