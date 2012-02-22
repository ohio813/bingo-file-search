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
///:searchwidget.h
#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QTimer>
#include "../core/File.h"

namespace Ui
{
    class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget (QWidget *parent = 0);
    ~SearchWidget();
    bool isRegularMatchFilterOn();
    bool isFullFathMatchFilterOn();
    bool isDateModifiedFilterOn();
    bool isDateCreatedFilterOn();
    bool isSizeFilterOn();
    bool isFileTypeFilterOn();
    bool isReadOnlyOn();
    bool isHiddenOn();
    bool isSystemOn();
    bool isDirectoryOn();
    TIME32 getDateModifiedBegin();
    TIME32 getDateModifiedEnd();
    TIME32 getDateCreatedBegin();
    TIME32 getDateCreatedEnd();
    DWORD getSizeFilterBegin();
    DWORD getSizeFilterEnd();
    QString getSearchKeyWord();

public slots:
    //language refresh
    void languageRefresh();
    void changeCenterWidgetReq();
	void refreshSearchResReq();
	void refreshSearchRes();
signals:
    void explains();
    void changeCenterWidget (bool isSearchwidget);
private:
    Ui::SearchWidget *ui;
	QTimer *timer;
};

#endif // SEARCHWIDGET_H
///:~
