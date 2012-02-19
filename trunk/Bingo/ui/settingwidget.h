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
///:settingwidget.h
#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

namespace Ui
{
    class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget (QWidget *parent = 0);
    ~SettingWidget();
public slots:
    //language refresh
    void languageRefresh();
	void langApply();
    void changeCenterWidgetReq();
signals:
    void explains();
    void changeCenterWidget (bool isSearchwidget);
private:
    Ui::SettingWidget *ui;
};

#endif // WIDGET_H
///:~
