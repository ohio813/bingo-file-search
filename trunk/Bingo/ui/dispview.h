#ifndef DISPVIEW_H
#define DISPVIEW_H

#include <QWidget>
#include <QTableView>
#include<QString>
#include<QSqlDatabase>
#include"DispModel.h"

class DispView : public QTableView
{
    Q_OBJECT
public:
    explicit DispView(QWidget *parent = 0);
    DispView(QSqlDatabase &database);
    void refresh();
private:
    void setSql();
    DispModel model;
    QString sql;
    QSqlDatabase db;


signals:

public slots:

};

#endif // DISPVIEW_H
