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

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

void SearchWidget::languageRefresh()
{
    ui->lineEdit->setPlaceholderText(tr("Enter to Search"));
    ui->pushButton_3->setToolTip(tr("Setting"));
    ui->label->setText(tr("Search filter:"));
    ui->label_3->setText(tr("From:"));
    ui->label_5->setText(tr("To:"));
    ui->label_9->setText(tr("From:"));
    ui->label_15->setText(tr("To:"));
    ui->label_8->setText(tr("From:"));
    ui->label_14->setText(tr("To:"));
    ui->toolBox->setItemText(0,tr("Basic"));
    ui->toolBox->setItemText(1,tr("Date modified"));
    ui->toolBox->setItemText(2,tr("Date created"));
    ui->toolBox->setItemText(3,tr("Size"));
    ui->toolBox->setItemText(4,tr("File type"));
    ui->radioButton_3->setText(tr("on"));
    ui->radioButton_4->setText(tr("on"));
    ui->radioButton->setText(tr("on"));
    ui->radioButton_5->setText(tr("on"));
    ui->checkBox->setText(tr("Full path match"));
    ui->checkBox_2->setText(tr("Regular expression"));
    ui->checkBox_3->setText(tr("Read only"));
    ui->checkBox_4->setText(tr("Hidden"));
    ui->checkBox_5->setText(tr("System"));
    ui->checkBox_6->setText(tr("Directory"));


}

///:~
