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
///:shellmenudll.cpp
#define _AFXDLL
#define DLL_FILE
#include "shellmenudll.h"
#include <afxole.h>
#include "ShellContextMenu.h"

bool enable = false;

void initShellMenuDll()
{
    if (!enable)
    {
        AfxOleInit ();
        enable = true;
    }
}

void getShellMenu (QString filePath, HWND hWnd, QPoint pos)
{
    if (!enable)
        initShellMenuDll();

    CShellContextMenu cShellContextMenu;
    cShellContextMenu.SetObjects (filePath);
    cShellContextMenu.ShowContextMenu (CWnd::FromHandle (hWnd), CPoint (pos.x(), pos.y()));
}

void getShellMenuEx (QVector<QString> filePath, HWND hWnd, QPoint pos)
{
    if (!enable)
        initShellMenuDll();

    CShellContextMenu cShellContextMenu;
    cShellContextMenu.SetObjects (filePath);
    cShellContextMenu.ShowContextMenu (CWnd::FromHandle (hWnd), CPoint (pos.x(), pos.y()));
}

///:~