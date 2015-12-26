#include "lavelhistory.h"
#include "ipwindow.h"

#include <QtWidgets/QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QSqlQuery>


lavelhistory::lavelhistory(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	parent=NULL;
	phone.clear();

	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|Qt::WindowSystemMenuHint|Qt::WindowTitleHint);
	this->setFixedSize(400,350);

	
	
	
	
}

lavelhistory::~lavelhistory()
{
	
}
void lavelhistory::setpParent(IPWindow* prt)
{
	parent=(IPWindow*)prt;
}
void lavelhistory::iShow()
{
	QString str;
	str=tr("Качество связи для ")+name+" ("+phone+")";
	this->setWindowTitle(str);
	int i=0,devCount=0,lavel=0;
	str.clear();
	str="select timestamp,dev_id, name, phone, lavel from lavel_gsm where phone ='"+phone+"' order by timestamp asc";
	QSqlQuery *tq = new QSqlQuery();
	if (!tq->exec(str))
	{
		QMessageBox::critical(0, qApp->tr("Системная ошибка"),tr("      Ошибка чтения данных lavel_gsm!"),
						QMessageBox::Ok,QMessageBox::NoButton);
		hide();
		return;
	}
	else
	{
		
		while (tq->next())
		{
			devCount++;
		}
		
		ui.tableWidget->setSortingEnabled(FALSE);
		ui.tableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );

		ui.tableWidget->clear();
		QTableWidgetItem *tItem;
		
		
		ui.tableWidget->setColumnCount(3);
		ui.tableWidget->setRowCount(devCount);
		tItem = new QTableWidgetItem(tr("№ п/п"));
		ui.tableWidget->setHorizontalHeaderItem(0,tItem);
		tItem = new QTableWidgetItem(tr("Время/Дата"));
		ui.tableWidget->setHorizontalHeaderItem(1,tItem);
		tItem = new QTableWidgetItem(tr("Уровень сигнала (%)"));
		ui.tableWidget->setHorizontalHeaderItem(2,tItem);
		tq->first();
		while (i<devCount)
		{
			//INDEX
			tItem = new QTableWidgetItem(i+1);
			tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tableWidget->setItem(i,0,tItem);
			ui.tableWidget->setColumnHidden(0,true);
			
			//time
			str=tq->value(0).toString();
			str=str.replace("T"," ");
			tItem = new QTableWidgetItem(str);
			tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tableWidget->setItem(i,1,tItem);
			//lavel
			lavel=(tq->value(4).toInt()*100)/30;
			if(lavel>100)
				lavel=100;
			tItem = new QTableWidgetItem(QString::number(lavel,10));
			tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tableWidget->setItem(i,2,tItem);
			i++;
			tq->next();
		}
		ui.tableWidget->resizeColumnsToContents();
		ui.tableWidget->resizeRowsToContents();
		ui.tableWidget->resizeRowsToContents();
		ui.tableWidget->setColumnWidth(1,160);
		ui.tableWidget->setColumnWidth(2,155);
		ui.tableWidget->setSortingEnabled(TRUE);
		ui.tableWidget->sortItems(1,Qt::AscendingOrder);
	
	delete tq;
	show();
	}
}

void lavelhistory::on_pushButton_clicked()
{
	hide();
}
