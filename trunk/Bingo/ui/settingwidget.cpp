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
///:settingwidget.cpp
#include "settingwidget.h"
#include "ui_settingwidget.h"

SettingWidget::SettingWidget (QWidget *parent) :
    QWidget (parent),
    ui (new Ui::SettingWidget)
{
    ui->setupUi (this);
    connect (ui->pushButton_2, SIGNAL (clicked()), this, SLOT (changeCenterWidgetReq()));
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget:: languageRefresh()
{
    ui->groupBox->setTitle (tr ("Region and Language"));
    ui->label_2->setText (tr ("Choose a display language:"));
    ui->pushButton->setText (tr ("Apply"));
    ui->pushButton_2->setToolTip (tr ("Search"));
    ui->groupBox_2->setTitle (tr ("Search Region"));
    ui->label_3->setText (tr ("Search results in:"));
    ui->groupBox_3->setTitle (tr ("About"));
}

void SettingWidget::changeCenterWidgetReq()
{
    emit changeCenterWidget (false);
}
///:~
