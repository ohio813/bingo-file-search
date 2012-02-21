#include "DispModel.h"
#include "File.h"

DispModel::DispModel(QObject *parent):QSqlQueryModel(parent)
{
}


QVariant DispModel::data(const QModelIndex &index,int role) const
{
    QVariant value=QSqlQueryModel::data(index,role);

    if(!index.isValid())
    {
        return QVariant();
    }
    if(role==Qt::TextAlignmentRole)
    {
		if(index.column()==0)
		{
			return int(Qt::AlignCenter);
		}
		if(index.column()==1||index.column()==2)
		{
			return int(Qt::AlignLeft|Qt::AlignVCenter);
		}
		else
		{
			return int(Qt::AlignRight|Qt::AlignVCenter);
		}
    }
    else if(role==Qt::DisplayRole)
    {
        if (index.column()==0)
	{
            //TODO: display icons
            return "ICONS";
	}
        else if (index.column()==3)
        {
			QString dispNum=value.toString();
			QString dispKB=" KB";
			return dispNum+dispKB;
        }
        else if(index.column()==4)
        {
            TIME32 time=value.toInt();
			SYSTEMTIME sysTime;
			TIME32toSYSTIME(time,sysTime);
			QString _year=number(sysTime.wYear);
			QString _month=number(sysTime.wMonth);
			if (_month.length()==1)
			{
				_month.insert(0,'0');
			}
			QString _day=number(sysTime.wDay);
			if (_day.length()==1)
			{
				_day.insert(0,'0');
			}
			QString _hour=number(sysTime.wHour);
			if (_hour.length()==1)
			{
				_hour.insert(0,'0');
			}
			QString _minute=number(sysTime.wMinute);
			if (_minute.length()==1)
			{
				_minute.insert(0,'0');
			}
			return _year+'/'+_month+'/'+_day+' '+_hour+':'+_minute;
        }
        else
        {
            return value;
        }
    }
    return QVariant();
}

void DispModel::sort(int column, Qt::SortOrder order)
{
	QString _order="select FRN,FNAME,FRN,fileSize,fileTime from [DISP] order by ";
	switch (column)
	{
	case 0:return;
	case 1:_order+="FNAME ";
		break;
	case 2:_order+="FRN ";
		break;
	case 3:_order+="fileSize ";
		break;
	case 4:_order+="fileSize ";
		break;
	}
	if(order==Qt::DescendingOrder)
	{
		_order+="DESC";
	}
	this->setQuery(_order);
}
