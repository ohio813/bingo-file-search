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
///:settingwidget.cpp
#include "settingwidget.h"
#include "ui_settingwidget.h"
#include "../core/Data.h"
#include "../core/VolumeLetter.h"
#include "../core/File.h"
#include "Language.h"
#include <vector>
using namespace std;

VolSettingBtn::VolSettingBtn (char Path, State state, QWidget *parent) :
    QPushButton (parent)
{
    m_path = Path;
    m_state = state;
    setOutLook();
}

void VolSettingBtn::setOutLook()
{
    switch (m_state)
    {
        case Active:
            setText (tr ("Actived"));
            setStyleSheet ("background:lime");
            repaint();
            setEnabled (true);
            break;

        case Deactive:
            setText (tr ("Deactived"));
            setStyleSheet ("background:red");
            repaint();
            setEnabled (true);
            break;

        case NotSupport:
            setText (tr ("Not Support"));
            setStyleSheet ("background:white;color:gray");
            repaint();
            setEnabled (false);
            break;
    }
}

void VolSettingBtn::setHoverOutLook()
{
    switch (m_state)
    {
        case Active:
            setText (tr ("Deactive"));
            setStyleSheet ("background:red");
            repaint();
            setEnabled (true);
            break;

        case Deactive:
            setText (tr ("Active"));
            setStyleSheet ("background:lime");
            repaint();
            setEnabled (true);
            break;

        case NotSupport:
            setText (tr ("Not Support"));
            setStyleSheet ("background:white;color:gray");
            repaint();
            setEnabled (false);
            break;
    }
}

bool VolSettingBtn::event (QEvent * e)
{
    switch (e->type())
    {
        case QEvent::HoverEnter:
            setHoverOutLook();
            return QPushButton::event (e);

        case QEvent::HoverLeave:
            setOutLook();
            return QPushButton::event (e);

        case QEvent::MouseButtonRelease:
            if (m_state == Active)
            {
                data_coreMgr->DisActive (m_path);

                if (data_coreMgr->listCurVols().contains (m_path))
                    m_state = Active;
                else
                    m_state = Deactive;

                setOutLook();
            }
            else if (m_state == Deactive)
            {
                data_coreMgr->Active (m_path);

                if (data_coreMgr->listCurVols().contains (m_path))
                    m_state = Active;
                else
                    m_state = Deactive;

                setOutLook();
            }

            return QPushButton::event (e);

        default:
            return QPushButton::event (e);
    }
}

SettingWidget::SettingWidget (QWidget *parent) :
    QWidget (parent),
    ui (new Ui::SettingWidget)
{
    ui->setupUi (this);
    connect (ui->pushButton_2, SIGNAL (clicked()), this, SLOT (changeCenterWidgetReq()));
    int curLangIndex = 0, tmp_LangIndex = 0;
    QLocale curLang = uidata_Lang->getCurLang();
    foreach (QLocale locale, uidata_Lang->listAllLang())
    {
        ui->comboBox->addItem (QString ("%1(%2)").arg (QLocale::languageToString (locale.language()), QLocale::countryToString (locale.country())));

        if (locale == curLang)
            curLangIndex = tmp_LangIndex;
        else tmp_LangIndex++;
    }
    ui->comboBox->setCurrentIndex (curLangIndex);
    connect (ui->pushButton, SIGNAL (clicked()), this, SLOT (langApply()));
}

void SettingWidget::setTable()
{
    QList<char> activePath = data_coreMgr->listCurVols();
    int curRow = 0;

    for (std::set<VolInfoNode, std::less<VolInfoNode>>::iterator ptr = data_VolInfos->m_volinfos.begin()
            ; ptr != data_VolInfos->m_volinfos.end(); ++ptr)
    {
        if (!ptr->isMounted) continue;

        ui->tableWidget->insertRow (curRow);
        char _Path = WChartoCharLetter (ptr->Path);
        ui->tableWidget->setItem (curRow, 0, new QTableWidgetItem (uidata_iconCahe->getDiskIcon (ptr->Path), QString ("%1:\\").arg (_Path)));
        ui->tableWidget->setItem (curRow, 1, new QTableWidgetItem (QString::fromStdWString (ptr->VolumeLabel)));
        ui->tableWidget->setItem (curRow, 2, new QTableWidgetItem (QString::fromStdWString (ptr->FileSysType)));
        ui->tableWidget->setItem (curRow, 3, new QTableWidgetItem (VolSizeinStr(ptr->totalspace)));
        ui->tableWidget->setItem (curRow, 4, new QTableWidgetItem (VolSizeinStr(ptr->freespace)));

        if (ptr->isLocalDisk & ptr->isNTFS)
        {
            if (activePath.contains (_Path))
                ui->tableWidget->setIndexWidget (ui->tableWidget->model()->index (curRow, 5), new VolSettingBtn (_Path, VolSettingBtn::Active));
            else
                ui->tableWidget->setIndexWidget (ui->tableWidget->model()->index (curRow, 5), new VolSettingBtn (_Path, VolSettingBtn::Deactive));
        }
        else
            ui->tableWidget->setIndexWidget (ui->tableWidget->model()->index (curRow, 5), new VolSettingBtn (_Path, VolSettingBtn::NotSupport));

        curRow++;
    }
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget:: languageRefresh()
{
    ui->groupBox->setTitle (tr ("Region and Language"));
    ui->label_2->setText (tr ("Choose a display language:"));
    ui->pushButton->setText (tr ("Apply"));
    ui->pushButton_2->setToolTip (tr ("Search"));
    ui->groupBox_2->setTitle (tr ("Search Region"));
    ui->label_3->setText (tr ("Search results in:"));
    ui->groupBox_3->setTitle (tr ("About"));
    QStringList tableHeader;
    tableHeader.push_back (tr ("Volume"));
    tableHeader.push_back (tr ("Volume Name"));
    tableHeader.push_back (tr ("File System"));
    tableHeader.push_back (tr ("Total Size"));
    tableHeader.push_back (tr ("State"));
    ui->tableWidget->setHorizontalHeaderLabels (QStringList());
}

void SettingWidget::changeCenterWidgetReq()
{
    emit changeCenterWidget (false);
}

void SettingWidget::langApply()
{
    vector<QLocale> allLang = uidata_Lang->listAllLang();
    int newLang = ui->comboBox->currentIndex();

    if (allLang[newLang] != uidata_Lang->getCurLang())
    {
        uidata_Lang->setCurLang (allLang[newLang]);
        int curLangIndex = 0, tmp_LangIndex = 0;
        QLocale curLang = uidata_Lang->getCurLang();
        foreach (QLocale locale, uidata_Lang->listAllLang())
        {
            if (locale == curLang)
                curLangIndex = tmp_LangIndex;
            else tmp_LangIndex++;
        }
        ui->comboBox->setCurrentIndex (curLangIndex);
    }
}
///:~
