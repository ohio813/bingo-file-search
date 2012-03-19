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
///:shellmenudll.h
#ifndef SHELLMENUDLL_H
#define SHELLMENUDLL_H

#include <QtCore>
#include <QVector>

#if defined _AFX || defined _AFXDLL //MFC
#include<afx.h>
#else
#include<Windows.h>
#endif

#ifdef DLL_FILE
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

    DLL void initShellMenuDll();
    DLL void getShellMenu (QString filePath, HWND hWnd, QPoint pos);
    DLL void getShellMenuEx (QVector<QString> filePath, HWND hWnd, QPoint pos);

#ifdef __cplusplus
}
#endif

#endif

///:~