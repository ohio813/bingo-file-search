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
///:dispview.h

#ifndef DISPVIEW_H
#define DISPVIEW_H

#include <QWidget>
#include <QTableView>
#include<QString>
#include<QSqlDatabase>
#include"DispModel.h"

extern SearchWidget *uidata_searchwidget;

class DispView : public QTableView
{
    Q_OBJECT
public:
    explicit DispView (QWidget *parent = 0);
    DispView (QSqlDatabase &database);
    void refresh();
private:
    void setSql();
    DispModel model;
    QString sql;


signals:

public slots:

};

#endif

///:~
