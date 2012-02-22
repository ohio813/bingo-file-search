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
///:mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include "searchwidget.h"
#include "settingwidget.h"
#include "loadingwidget.h"
#include "waitingwidget.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow (QWidget *parent = 0);
    ~MainWindow();
public slots:
    void changeCenterWidget (bool isSearchwidget);
    //System Tray Icon
    void onSystemTrayIconClicked (QSystemTrayIcon::ActivationReason reason);
    //connect to other instance of this app, to ensure there's only one instance running in Desktop.
    void newLocalSocketConnection();
    //language refresh
    void languageRefresh();
    //app initial
    void appInitStart();
	void appInitEnd();
	void showWaiting();
	void hideWaiting();
	void onQuit();
signals:
    void explains();
    void signalResize (QResizeEvent *);
protected:
    void resizeEvent (QResizeEvent *);
	static DWORD WINAPI QuitFunc(LPVOID in);
private:
    Ui::MainWindow *ui;
    //System Tray Icon
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *trayIconMenuActionRestore;
    QAction *trayIconMenuActionQuit;
    //connect to other instance of this app, to ensure there's only one instance running in Desktop.
    QLocalServer *localServer;
    //Widget
    bool isSearchwidget;
    SearchWidget *searchwidget;
    SettingWidget *settingwidget;
    LoadingWidget *loadingwidget;
    WaitingWidget *waitingwidget;
    QStackedWidget *stackedWidget;
};

#endif // MAINWINDOW_H
///:~
