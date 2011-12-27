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
///:mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Language.h"
#include "util\Log.h"
#include "core\Data.h"

MainWindow::MainWindow (QWidget *parent) :
    QMainWindow (parent),
    ui (new Ui::MainWindow)
{
    ui->setupUi (this);
    //set language refresh
    connect (uidata_Lang, SIGNAL (refreshLangRequest()), this, SLOT (languageRefresh()));
    //setup System Tray Icon
    trayIcon = new QSystemTrayIcon (this);
    trayIcon->setIcon (QIcon (":/image/logo.png"));
    trayIcon->show();
    trayIcon->setToolTip (tr ("Bingo!"));
    trayIconMenu = new QMenu (this);
    trayIconMenuActionRestore = new QAction (tr ("Restore"), this);
    connect (trayIconMenuActionRestore, SIGNAL (triggered()), this, SLOT (showNormal()));
    trayIconMenu->addAction (trayIconMenuActionRestore);
    trayIconMenuActionQuit = new QAction (tr ("Quit"), this);
    connect (trayIconMenuActionQuit, SIGNAL (triggered()), qApp, SLOT (quit()));
    trayIconMenu->addAction (trayIconMenuActionQuit);
    trayIcon->setContextMenu (trayIconMenu);
    connect (trayIcon, SIGNAL (activated (QSystemTrayIcon::ActivationReason)), this,
             SLOT (onSystemTrayIconClicked (QSystemTrayIcon::ActivationReason)));
    QApplication::setQuitOnLastWindowClosed (false);
    //connect to other instance of this app, to ensure there's only one instance running in Desktop.
    localServer = new QLocalServer (this);
    connect (localServer, SIGNAL (newConnection()), this, SLOT (newLocalSocketConnection()));

    if (!localServer->listen ("Bingo-File-Search"))
    {
        if (localServer->serverError() == QAbstractSocket::AddressInUseError
                && QFile::exists (localServer->serverName()))
        {
            QFile::remove (localServer->serverName());
            localServer->listen ("Bingo-File-Search");
        }
    }
}

MainWindow::~MainWindow()
{
    delete localServer;
    delete trayIconMenuActionQuit;
    delete trayIconMenuActionRestore;
    delete trayIconMenu;
    delete trayIcon;
    delete ui;
}

void MainWindow::onSystemTrayIconClicked (QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this->setWindowState (Qt::WindowActive);
            this->showNormal();
            break;

        default:
            break;
    }
}

void MainWindow::newLocalSocketConnection()
{
    this->setWindowState (Qt::WindowActive);
    this->showNormal();
}

void MainWindow::languageRefresh()
{
    trayIcon->setToolTip (tr ("Bingo!"));
    trayIconMenuActionRestore->setText (tr ("Restore"));
    trayIconMenuActionQuit->setText (tr ("Quit"));
}
///:~