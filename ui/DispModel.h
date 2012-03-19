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
///:DispModel.h
#ifndef DISPMODEL_H
#define DISPMODEL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class DispModel : public QSqlQueryModel, QObject
{
    Q_OBJECT
public:
    DispModel (QObject *parent = 0);
    void sort (int column, Qt::SortOrder order);
    QString sql;
private:
    QVariant data (const QModelIndex &index, int role) const;
signals:
    void explains();
    void waitBegin ();
    void waitEnd ();
};

#endif

///:~
