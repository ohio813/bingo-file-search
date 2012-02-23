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
///:CoreMgr.h
#ifndef COREMGR_H
#define COREMGR_H

#include <QtCore>
#include <QMap>
#include "VolumeMgr.h"
#include "USN.h"

class CoreMgr: public QThread
{
    Q_OBJECT
public:
    CoreMgr();
    ~CoreMgr();
	void run();
	QList<char> listCurVols();
	void Active(char Path);
	void DisActive(char Path);
signals:
    void explains();
    void appInitStart();
    void appInitProgress (int percent, QString detail);
	void appInitEnd(bool gotosearch);
	void beginWait();
	void endWait();
	void refreshSearch();
public slots:
private:
    QMap<char, VolUSN*> m_VolUsns;
};

#endif
///:~