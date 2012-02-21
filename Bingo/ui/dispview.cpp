#include"dispview.h"

DispView::DispView(QSqlDatabase &database)
{
	this->db=database;
	this->setAlternatingRowColors(true);
	this->setSelectionBehavior(QTableView::SelectRows);
	this->setShowGrid(true);
	this->verticalHeader()->setVisible(false);
	this->setSortingEnabled(true);
}

void DispView::refresh()
{
    setSql();
    this->model.setQuery(sql,db);
    this->model.setQuery("select FRN,FNAME,FRN,fileSize,fileTime from [DISP]",db);
	this->model.setHeaderData(0,Qt::Horizontal,"",Qt::DisplayRole);
	this->model.setHeaderData(1,Qt::Horizontal,"File Name",Qt::DisplayRole);
	this->model.setHeaderData(2,Qt::Horizontal,"Path",Qt::DisplayRole);
	this->model.setHeaderData(3,Qt::Horizontal,"File Size",Qt::DisplayRole);
	this->model.setHeaderData(4,Qt::Horizontal,"Edit Time",Qt::DisplayRole);
    this->setModel(&model);
    this->show();
}

void DispView::setSql()
{
    this->sql="create view [DISP] as select FNAME,FRN,fileSize,fileTime from C";
}
