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
///:dispview.cpp
#include "dispview.h"
#include <QHeaderView>
#include "searchwidget.h"
#include <QList>
#include "../core/Data.h"

SearchWidget *uidata_searchwidget;


DispView::DispView (QWidget *parent) : QTableView (parent)
{
    this->setAlternatingRowColors (true);
    this->setSelectionBehavior (QTableView::SelectRows);
    this->setShowGrid (true);
    this->verticalHeader()->setVisible (false);
    this->setSortingEnabled (true);
    connect (this, SIGNAL (doubleClicked (QModelIndex)), SLOT (slotDoubleClick (QModelIndex)));
}

void DispView::refresh()
{
    setSql();
    data_masterDB->beginread();
    this->model.setQuery (this->model.sql, data_masterDB->getDB());
    this->model.setHeaderData (0, Qt::Horizontal, "", Qt::DisplayRole);
    this->model.setHeaderData (1, Qt::Horizontal, tr ("File Name"), Qt::DisplayRole);
    this->model.setHeaderData (2, Qt::Horizontal, tr ("Path"), Qt::DisplayRole);
    this->model.setHeaderData (3, Qt::Horizontal, tr ("File Size"), Qt::DisplayRole);
    this->model.setHeaderData (4, Qt::Horizontal, tr ("Create Time"), Qt::DisplayRole);
    this->model.setHeaderData (5, Qt::Horizontal, tr ("Edit Time"), Qt::DisplayRole);
    this->setModel (&model);
    this->show();
    data_masterDB->endread();
}

void DispView::setSql()
{
    this->model.sql = "";
    QString _basic_sql =
        "select Master_%1.frn,Master_%1.name,Path_%1.path,"
        "Master_%1.size,Master_%1.createTime,Master_%1.writeTime from Master_%1 "
        "inner join Path_%1 on Master_%1.frn=Path_%1.frn ";
    QString pathFilter, sizeFilter, createTimeFilter, writeTimeFilter;

    if (!uidata_searchwidget->getSearchKeyWord().isEmpty())
    {
        if (!uidata_searchwidget->isFullPathMatchFilterOn())
        {
            pathFilter = "Master_%1.name like \"%%2%\" ";
            pathFilter = pathFilter.arg ("%1", uidata_searchwidget->getSearchKeyWord());
        }

        if (uidata_searchwidget->isFullPathMatchFilterOn())
        {
            pathFilter = "Path_%1.path like \"%%2%\" ";
            pathFilter = pathFilter.arg ("%1", uidata_searchwidget->getSearchKeyWord());
        }
    }

    if (uidata_searchwidget->isSizeFilterOn())
    {
        QString _low_size = QString::number (uidata_searchwidget->getSizeFilterBegin());
        QString _hi_size = QString::number (uidata_searchwidget->getSizeFilterEnd());
        sizeFilter = "Master_%1.size>%2 and Master_%1.size<%3";
        sizeFilter = sizeFilter.arg ("%1", _low_size, _hi_size);
    }

    if (uidata_searchwidget->isDateCreatedFilterOn())
    {
        QString _begin_create_date = QString::number (uidata_searchwidget->getDateCreatedBegin());
        QString _end_create_date = QString::number (uidata_searchwidget->getDateCreatedEnd());
        createTimeFilter = "Master_%1.createTime>%2 and Master_%1.createTime<%3";
        createTimeFilter = createTimeFilter.arg ("%1", _begin_create_date, _end_create_date);
    }

    if (uidata_searchwidget->isDateModifiedFilterOn())
    {
        QString _begin_mod_time = QString::number (uidata_searchwidget->getDateModifiedBegin());
        QString _end_mod_time = QString::number (uidata_searchwidget->getDateModifiedEnd());
        writeTimeFilter = "Master_%1.writeTime>%2 and Master_%1.writeTime<%3";
        writeTimeFilter = writeTimeFilter.arg ("%1", _begin_mod_time, _end_mod_time);
    }

    bool needAnd = false;

    if ( (! pathFilter.isEmpty())
            || uidata_searchwidget->isSizeFilterOn()
            || uidata_searchwidget->isDateCreatedFilterOn()
            || uidata_searchwidget->isDateModifiedFilterOn())
    {
        _basic_sql = _basic_sql + "where ";
    }

    _basic_sql = _basic_sql + pathFilter;
    needAnd = !pathFilter.isEmpty();

    if (uidata_searchwidget->isSizeFilterOn())
    {
        if (needAnd)
        {
            _basic_sql = _basic_sql + "and " + sizeFilter;
        }
        else
        {
            _basic_sql = _basic_sql + sizeFilter;
            needAnd = true;
        }
    }

    if (uidata_searchwidget->isDateCreatedFilterOn())
    {
        if (needAnd)
        {
            _basic_sql = _basic_sql + "and " + createTimeFilter;
        }
        else
        {
            _basic_sql = _basic_sql + createTimeFilter;
            needAnd = true;
        }
    }

    if (uidata_searchwidget->isDateModifiedFilterOn())
    {
        if (needAnd)
        {
            _basic_sql = _basic_sql + "and " + writeTimeFilter;
        }
        else
        {
            _basic_sql = _basic_sql + writeTimeFilter;
            needAnd = true;
        }
    }

    QList<char> _drivers = data_coreMgr->listCurVols();

    for (QList<char>::iterator _ptr = _drivers.begin();
            _ptr != _drivers.end();
            _ptr++)
    {
        this->model.sql = (this->model.sql) + _basic_sql.arg (*_ptr) + " union ";
    }

    this->model.sql = this->model.sql.mid (0, model.sql.length() - 6);
}

void DispView::slotDoubleClick (const QModelIndex &index)
{
    QModelIndex _index = index.sibling (index.row(), 2);
    QString _fileName = _index.data().toString();
    ShellExecute (NULL, L"open", _fileName.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

///:~
