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
///:searchwidget.cpp
#include "searchwidget.h"
#include "ui_searchwidget.h"

SearchWidget::SearchWidget (QWidget *parent) :
    QWidget (parent),
    ui (new Ui::SearchWidget)
{
    ui->setupUi (this);
    ui->toolBox->setCurrentIndex (0);
    connect (ui->pushButton_3, SIGNAL (clicked()), this, SLOT (changeCenterWidgetReq()));
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

void SearchWidget::languageRefresh()
{
    ui->lineEdit->setPlaceholderText (tr ("Enter to Search"));
    ui->pushButton_3->setToolTip (tr ("Setting"));
    ui->label->setText (tr ("Search filter:"));
    ui->label_3->setText (tr ("From:"));
    ui->label_5->setText (tr ("To:"));
    ui->label_9->setText (tr ("From:"));
    ui->label_15->setText (tr ("To:"));
    ui->label_8->setText (tr ("From:"));
    ui->label_14->setText (tr ("To:"));
    ui->toolBox->setItemText (0, tr ("Basic"));
    ui->toolBox->setItemText (1, tr ("Date modified"));
    ui->toolBox->setItemText (2, tr ("Date created"));
    ui->toolBox->setItemText (3, tr ("Size"));
    ui->toolBox->setItemText (4, tr ("File type"));
    ui->radioButton_3->setText (tr ("on"));
    ui->radioButton_4->setText (tr ("on"));
    ui->radioButton->setText (tr ("on"));
    ui->radioButton_5->setText (tr ("on"));
    ui->checkBox->setText (tr ("Full path match"));
    ui->checkBox_2->setText (tr ("Regular expression"));
    ui->checkBox_3->setText (tr ("Read only"));
    ui->checkBox_4->setText (tr ("Hidden"));
    ui->checkBox_5->setText (tr ("System"));
    ui->checkBox_6->setText (tr ("Directory"));
}

void SearchWidget::changeCenterWidgetReq()
{
	emit changeCenterWidget(true);
}

bool SearchWidget::isRegularMatchFilterOn()
{
    return ui->checkBox_2->isChecked();
}

bool SearchWidget::isFullFathMatchFilterOn()
{
    return ui->checkBox->isChecked();
}

bool SearchWidget::isDateModifiedFilterOn()
{
    return ui->radioButton_3->isChecked();
}

bool SearchWidget::isDateCreatedFilterOn()
{
    return ui->radioButton_5->isChecked();
}

bool SearchWidget::isSizeFilterOn()
{
    return ui->radioButton->isChecked();
}

bool SearchWidget::isFileTypeFilterOn()
{
    return ui->radioButton_4->isChecked();
}

bool SearchWidget::isReadOnlyOn()
{
    return ui->checkBox_3->isChecked();
}

bool SearchWidget::isHiddenOn()
{
    return ui->checkBox_4->isChecked();
}

bool SearchWidget::isSystemOn()
{
    return ui->checkBox_5->isChecked();
}

bool SearchWidget::isDirectoryOn()
{
    return ui->checkBox_6->isChecked();
}

TIME32 SearchWidget::getDateModifiedBegin()
{
    return QTIMEtoTIME32 (ui->dateTimeEdit->dateTime());
}

TIME32 SearchWidget::getDateModifiedEnd()
{
    return QTIMEtoTIME32 (ui->dateTimeEdit_2->dateTime());
}

TIME32 SearchWidget::getDateCreatedBegin()
{
    return QTIMEtoTIME32 (ui->dateTimeEdit_3->dateTime());
}

TIME32 SearchWidget::getDateCreatedEnd()
{
    return QTIMEtoTIME32 (ui->dateTimeEdit_4->dateTime());
}

DWORD SearchWidget::getSizeFilterBegin()
{
    switch (ui->comboBox_3->currentIndex())
    {
        case 0:
            return  static_cast<DWORD> (ui->doubleSpinBox_3->value());

        case 1:
            return static_cast<DWORD> (ui->doubleSpinBox_3->value() * 1024);

        case 2:
            return static_cast<DWORD> (ui->doubleSpinBox_3->value() * 1048576);
    }
}

DWORD SearchWidget::getSizeFilterEnd()
{
    switch (ui->comboBox_4->currentIndex())
    {
        case 0:
            return  static_cast<DWORD> (ui->doubleSpinBox_4->value());

        case 1:
            return static_cast<DWORD> (ui->doubleSpinBox_4->value() * 1024);

        case 2:
            return static_cast<DWORD> (ui->doubleSpinBox_4->value() * 1048576);
    }
}

QString SearchWidget::getSearchKeyWord()
{
    return ui->lineEdit->text();
}
///:~
