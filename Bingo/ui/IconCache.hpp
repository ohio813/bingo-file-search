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
///:IconCache.hpp
#ifndef ICONCACHE_HPP
#define ICONCACHE_HPP
#include <QtCore>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QHash>
#include <Windows.h>
#include <Shellapi.h>
#include "../core/Memory.h"
#pragma comment(lib,"Shell32.lib")

class IconCache
{
public:
    IconCache()
    {
        getFolderIcon();
    }
    ~IconCache()
    {
        m_data->freeClass(m_folderIcon);

        for (QHash<QString, QIcon*>::iterator ptr = m_fileExtIconMap.begin();
                ptr != m_fileExtIconMap.end(); ++ptr)
            m_data->freeClass(ptr.value());
    }
    QIcon* getIcon (QString fileName, DWORD fileAttrib)
    {
        if (fileAttrib & FILE_ATTRIBUTE_DIRECTORY)
            return m_folderIcon;
        else
        {
            QString fileExt;
            int _fileExt = fileName.lastIndexOf (".") ;

            if (_fileExt == -1 || _fileExt == fileName.length() - 1)
                fileExt = " ";
            else
                fileExt = fileName.mid (_fileExt + 1);

            QHash<QString, QIcon*>::iterator ptr;

            if ( (ptr = m_fileExtIconMap.find (fileExt)) == m_fileExtIconMap.end())
            {
                QIcon *qicon = getFileIcon (fileExt);
                m_fileExtIconMap.insert (fileExt, qicon);
                return qicon;
            }
            else
                return ptr.value();
        }
    }
    static QIcon getDiskIcon (wchar_t Path)
    {
        wchar_t _Path[] = L"X:\\";
        _Path[0] = Path;
        SHFILEINFO info;
        SHGetFileInfoW (_Path, FILE_ATTRIBUTE_DIRECTORY, &info,
                        sizeof (SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
        QIcon ret = QIcon (QPixmap::fromWinHICON (info.hIcon));
        DestroyIcon (info.hIcon);
        return std::move (ret);
    }
private:
    void getFolderIcon()
    {
        SHFILEINFO info;
        SHGetFileInfoW (L"folder", FILE_ATTRIBUTE_DIRECTORY, &info,
                        sizeof (SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
        m_folderIcon = m_data->mallocClass<QIcon,QPixmap>(QPixmap::fromWinHICON (info.hIcon));
        DestroyIcon (info.hIcon);
    }
    QIcon* getFileIcon (QString fileExt)
    {
        SHFILEINFO info;
        SHGetFileInfoW (fileExt.toStdWString().c_str(), FILE_ATTRIBUTE_NORMAL, &info,
                        sizeof (SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
        QIcon * qicon = m_data->mallocClass<QIcon,QPixmap>(QPixmap::fromWinHICON (info.hIcon));
        DestroyIcon (info.hIcon);
        return qicon;
    }
private:
    QHash<QString, QIcon*> m_fileExtIconMap;
    QIcon *m_folderIcon;
	MemoryPool m_data;
};

#endif
///:~