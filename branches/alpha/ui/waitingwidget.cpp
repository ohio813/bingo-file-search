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
///:waitingwidget.cpp
#include "waitingwidget.h"
#include "ui_waitingwidget.h"

WaitingWidget::WaitingWidget (QWidget *parent) :
    QWidget (parent),
    ui (new Ui::WaitingWidget),
    background (new QLabel (this))
{
    ui->setupUi (this);
    movie = new QMovie (":/image/waiting.gif");
    ui->label->setMovie (movie);
    ui->label->setWindowOpacity (1);
    ui->label->raise();
    movie->start();
    background->setStyleSheet ("background-color: qlineargradient(spread:reflect, x1:0.920091, y1:0.114, x2:0, y2:1, stop:0.119318 rgba(10, 34, 255, 68), stop:1 rgba(0, 12, 227, 209))");
    background->lower();
    background->show();
    timer = new QTimer();
    timer->setSingleShot (true);
    connect (timer, SIGNAL (timeout ()), this, SLOT (show()));
}
void WaitingWidget::setSize (QResizeEvent *event)
{
    setGeometry (0, 0, event->size().width(), event->size().height());
    background->setGeometry (0, 0, size().width(), size().height());
}
WaitingWidget::~WaitingWidget()
{
    delete ui;
}
void WaitingWidget::Begin()
{
    timer->start (1500);
}
void WaitingWidget::End()
{
    if (timer->isActive())
        timer->stop();

    hide();
}
///:~