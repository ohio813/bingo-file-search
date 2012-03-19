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
///:DispModel.cpp
#include "DispModel.h"
#include "..\core\File.h"
#include "..\core\Data.h"

DispModel::DispModel (QObject *parent) : QSqlQueryModel (parent)
{
}


QVariant DispModel::data (const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data (index, role);

    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == 1 || index.column() == 2)
        {
            return int (Qt::AlignLeft | Qt::AlignVCenter);
        }
        else
        {
            return int (Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
            QModelIndex _index = index.sibling (index.row(), 1);
            return QVariant (*uidata_iconCahe->getIcon (_index.data().toString(), FILE_ATTRIBUTE_NORMAL));
        }
        else if (index.column() == 3)
            return FileSizeinStr (value.toInt());
        else if (index.column() == 4 || index.column() == 5)
            return TIME32toQStr (value.toInt());
        else
            return value;
    }

    return QVariant();
}

void DispModel::sort (int column, Qt::SortOrder order)
{
    QString _order = sql + "order by ";

    switch (column)
    {
        case 0:
            return;

        case 1:
            _order += "name ";
            break;

        case 2:
            _order += "path ";
            break;

        case 3:
            _order += "size ";
            break;

        case 4:
            _order += "createTime ";
            break;

        case 5:
            _order += "writeTime ";
            break;
    }

    if (order == Qt::DescendingOrder)
    {
        _order += "DESC";
    }

    data_masterDB->beginread();
    emit waitBegin();
    this->setQuery (_order, data_masterDB->getDB());
    emit waitEnd();
    data_masterDB->endread();
}
///:~