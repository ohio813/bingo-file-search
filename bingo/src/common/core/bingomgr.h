/**
*   Copyright (C) 2011-2012  Xu Cheng, Yang Zhengyu, Zuo Zhiheng, Yao Wenjie
*
*   This library is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This library is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.'
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:bingomgr.h
#ifndef BINGOMGR_H
#define BINGOMGR_H

#include "global.h"
#include <QtCore>
#include <QThread>

BINGO_FORWARD_DECLARE_CLASS (BingoMgr) // for moc generate

BINGO_BEGIN_HEADER
BINGO_BEGIN_NAMESPACE

class BingoMgr: private QThread
{
    Q_OBJECT
public:
private:
signals://Event Signal
    void explains();
public slots:
};

BINGO_END_NAMESPACE
BINGO_END_HEADER

#endif
///:~
