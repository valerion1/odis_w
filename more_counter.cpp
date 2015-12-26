#include "more_counter.h"
#include "datatypes.h"
#include "basesettings.h"
#include "ipwindow.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QSqlError>
#include <QSqlQuery>

more_counter::more_counter(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|Qt::WindowSystemMenuHint|Qt::WindowTitleHint);
	this->setFixedSize(400,300);

	list=NULL;
	list=new more_count();
	list->name.clear();
	
}

more_counter::~more_counter()
{
	if(list) delete list;
}

void more_counter::setpParent(IPWindow* prt)
{
	parent=(IPWindow*)prt;
}
void more_counter::setName(QString name)
{
	list->name.clear();
	list->name.append(name);
	setWindowTitle(tr("\"Показания счетчиков\" - ")+name);
}
void more_counter::GetData()
{
	QSqlQuery *tqDevice = new QSqlQuery();
	
	
	list->number=-1;
	list->type=-1;
	list->type=-1;
	list->counter_1_text.clear();
	list->counter_2_text.clear();
	list->counter_1_rm_text.clear();
	list->counter_2_rm_text.clear();
	if(!tqDevice->exec("select counter_number, counter_type, counter_precision, counter_1_text, counter_2_text, counter_1_rm_text, counter_2_rm_text  from device where name='"+list->name+"'"))
	{

		QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
			qApp->tr("Невозможно подключиться к БД"), QMessageBox::Cancel,
			QMessageBox::NoButton);

		delete tqDevice;
		
	}
	else
	{
		if (tqDevice->isActive())
		{
			//good
			while(tqDevice->next())
			{
				
				list->number=tqDevice->value(0).toInt();
				list->type=tqDevice->value(1).toInt();
				list->pres=tqDevice->value(2).toInt();
				list->counter_1_text=tqDevice->value(3).toString();
				list->counter_2_text=tqDevice->value(4).toString();
				list->counter_1_rm_text=tqDevice->value(5).toString();
				list->counter_2_rm_text=tqDevice->value(6).toString();
			}
		}
		delete tqDevice;
	}
	
}
void more_counter::iShow()
{

	GetData();


	QTableWidgetItem *tItem;

	ui.tableWidget->setColumnCount(4);
	ui.tableWidget->setRowCount(4);
	tItem = new QTableWidgetItem(tr("№ п/п"));
	ui.tableWidget->setHorizontalHeaderItem(0,tItem);
	tItem = new QTableWidgetItem(tr("Наименование параметра"));
	ui.tableWidget->setHorizontalHeaderItem(1,tItem);
	tItem = new QTableWidgetItem(tr("Ед.изм."));
	ui.tableWidget->setHorizontalHeaderItem(2,tItem);
	tItem = new QTableWidgetItem(tr("Значение"));
	ui.tableWidget->setHorizontalHeaderItem(3,tItem);
	

	//1
	tItem = new QTableWidgetItem("1");
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(0,0,tItem);

	tItem = new QTableWidgetItem(tr("Активная энергия по тарифу 1"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(0,1,tItem);


	tItem = new QTableWidgetItem(tr("кВТ"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(0,2,tItem);
			
	tItem = new QTableWidgetItem(QString::number(list->counter_1_text.toDouble(),',',list->pres));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(0,3,tItem);
	
	//2
	tItem = new QTableWidgetItem("2");
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(1,0,tItem);

	tItem = new QTableWidgetItem(tr("Активная энергия по тарифу 2"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(1,1,tItem);


	tItem = new QTableWidgetItem(tr("кВТ"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(1,2,tItem);
			
	tItem = new QTableWidgetItem(QString::number(list->counter_2_text.toDouble(),',',list->pres));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(1,3,tItem);

	//3
	tItem = new QTableWidgetItem("3");
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(2,0,tItem);

	tItem = new QTableWidgetItem(tr("Реактивная энергия по тарифу 1"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(2,1,tItem);


	tItem = new QTableWidgetItem(tr("кВТ"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(2,2,tItem);
			
	tItem = new QTableWidgetItem(QString::number(list->counter_1_rm_text.toDouble(),',',list->pres));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(2,3,tItem);


	//4
	tItem = new QTableWidgetItem("4");
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(3,0,tItem);

	tItem = new QTableWidgetItem(tr("Реактивная энергия по тарифу 2"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(3,1,tItem);


	tItem = new QTableWidgetItem(tr("кВТ"));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(3,2,tItem);
			
	tItem = new QTableWidgetItem(QString::number(list->counter_2_rm_text.toDouble(),',',list->pres));
	tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tableWidget->setItem(3,3,tItem);
	
	

	ui.tableWidget->setColumnWidth(0,40);
	ui.tableWidget->setColumnWidth(1,150);
	ui.tableWidget->setColumnWidth(2,60);
	ui.tableWidget->setColumnWidth(3,90);
	
	show();
}


void more_counter::on_pb_close_clicked()
{
	hide();
}


void more_counter::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		hide();
	}
	
	
}