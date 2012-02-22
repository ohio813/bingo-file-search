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
///:main.cpp
#include <QtGui/QApplication>
#include "ui/mainwindow.h"
#include "core/Data.h"
#include "util/Log.h"

int main (int argc, char *argv[])
{
    //initial app
    QApplication a (argc, argv);
    QTextCodec *utf8 = QTextCodec::codecForName ("utf-8");
    QTextCodec::setCodecForTr (utf8);
    QTextCodec::setCodecForLocale (utf8);
    QTextCodec::setCodecForCStrings (utf8);
    //check is there another instance of this app
    QLocalSocket socket;
    socket.connectToServer ("Bingo-File-Search");

    if (socket.waitForConnected (500))
    {
        a.quit();
        return 0;
    }

    //check args
    for (int i = 0; i < argc; ++i)
    {
        if (lstrcmpiA (argv[i], "--enable-log") == 0)
            Log::enable();//enable log
    }

    //initial global data
    InitGlobalData();
    //initial main windows
    uidata_Lang->loadLang();
    MainWindow w;
    w.show();
    data_coreMgr->start();
    return a.exec();;
}
///:~
