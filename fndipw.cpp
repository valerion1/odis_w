#include "fndipw.h"
#include "datatypes.h"
#include "basesettings.h"
#include "ipwindow.h"
#include "groupfindwidget.h"
#include "monitor.h"
#include <QRegExp>
#include <QtWidgets/QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>

fndIPW::fndIPW(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//connect(this, SIGNAL(clicked()),this, SLOT(close()));
	connect(ui.pb_close, SIGNAL(clicked()),this, SLOT(close()));
	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setChg(int)));

	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|/*Qt::WindowSystemMenuHint|*/Qt::WindowTitleHint);
	this->setWindowTitle(tr("Поиск ШУНО"));
   	this->setFixedSize(650,550);

	list=NULL;
	list = new QStringList();
	list->append(tr("Имя ШУНО"));
	list->append(tr("Адрес расположения ШУНО"));
	list->append(tr("Номер телефона ШУНО"));
	list->append(tr("Инвентарный номер ШУНО"));
	index=true;
	ipobj=NULL;
	ui.comboBox->addItem(list->at(0));
	ui.comboBox->addItem(list->at(1));
	ui.comboBox->addItem(list->at(2));
	ui.comboBox->addItem(list->at(3));
	group=NULL;
	group = new groupfindwidget(NULL);
	ui.tabWidget->removeTab(0); 
	ui.tabWidget->addTab(group,tr("Результаты поиска"));
}

fndIPW::~fndIPW()
{
	if(group)
		delete group;
}

void fndIPW::setChg(int pos)
{
	ui.line->clear();
	switch(pos)
	{
	case 0:
		{
			ui.lb_text->setText(tr("Имя ШУНО:"));
			break;
		}
	case 1:
		{
			ui.lb_text->setText(tr("Адрес расположения ШУНО:"));
			break;
		}
	case 2:
		{
			ui.lb_text->setText(tr("Номер телефона ШУНО:"));
			break;
		}
	case 3:
		{
			ui.lb_text->setText(tr("Инвентарный номер ШУНО:"));
			break;
		}
	}
		
	index=pos;
	ui.line->setFocus();
}
void fndIPW::setpParent(monitor* prt)
{
	parent=(monitor*)prt;
}
void fndIPW::setIndx(bool indx)
{
	index=indx;
}
void fndIPW::iShow()
{
	ui.line->clear();
	show();
	ui.line->setFocus();
}
void fndIPW::on_pb_find_clicked()
{
	
	for(int i=0;i<deviceList.size();i++)
	{
		group->DelDevice(deviceList.at(i)->getButton());
	}
	while (!deviceList.isEmpty())
	{	
		deviceList.removeFirst();
	}

	switch(parent->currMode)
	{
		case MODE_GG:
			parent->mode_sort=0;
			break;
		case MODE_GA:
			parent->mode_sort=1;
			break;
		case MODE_STATE:
			parent->mode_sort=2;
			break;
		case MODE_COUNT:
			parent->mode_sort=3;
			break;
	}
	parent->updateAfterConfig();	
	buildGroupList(ui.line->text());
	
}

void fndIPW::on_pb_close_clicked()
{
	hide();
	switch(parent->currMode)
	{
		case MODE_GG:
			parent->mode_sort=0;
			break;
		case MODE_GA:
			parent->mode_sort=1;
			break;
		case MODE_STATE:
			parent->mode_sort=2;
			break;
		case MODE_COUNT:
			parent->mode_sort=3;
			break;
	}
	parent->updateAfterConfig();
}

void fndIPW::buildGroupList(QString str)
{
	bool key_serch=false;
	if(str.isEmpty())
	{
		QMessageBox::critical(0, qApp->tr("Ошибка"),tr("По данному запросу ничего не найдено!"), QMessageBox::Ok,QMessageBox::NoButton);
		ui.line->clear();
		ui.line->setFocus();
	}
	else
	{
		shablon.setPatternSyntax(QRegExp::Wildcard);
		QString tmp="*"+str+"*";
		shablon.setPattern(tmp);
		
		switch(index)
		{
		case 0:
			{
				for(int i=0;i<parent->deviceList.count();i++)
				{
					if (shablon.exactMatch(parent->deviceList.at(i)->deviceDescr.name.toLower()))
					{	
						deviceList.append(parent->deviceList.at(i));
						group->AddDevice(parent->deviceList.at(i)->getButton());
						key_serch=true;
					}
				}
				break;
			}
		case 1:
			{
				for(int i=0;i<parent->deviceList.count();i++)
				{
					if (shablon.exactMatch(parent->deviceList.at(i)->deviceDescr.adress.toLower()))
					{	
						deviceList.append(parent->deviceList.at(i));
						group->AddDevice(parent->deviceList.at(i)->getButton());
						key_serch=true;
					}
				}
				break;
			}
		case 2:
			{
				for(int i=0;i<parent->deviceList.count();i++)
				{
					if (shablon.exactMatch(parent->deviceList.at(i)->deviceDescr.phone.toLower()))
					{	
						deviceList.append(parent->deviceList.at(i));
						group->AddDevice(parent->deviceList.at(i)->getButton());
						key_serch=true;
					}
				}
				break;
			}
		case 3:
			{
				for(int i=0;i<parent->deviceList.count();i++)
				{
					if (shablon.exactMatch(QString::number(parent->deviceList.at(i)->deviceDescr.dev_hard_number,10)))
					{	
						deviceList.append(parent->deviceList.at(i));
						group->AddDevice(parent->deviceList.at(i)->getButton());
						key_serch=true;
					}
				}
				break;
			}
		}
		if(key_serch)
		{
			ui.tabWidget->clear(); 
			ui.tabWidget->removeTab(0); 
			ui.tabWidget->addTab(group,tr("Результаты поиска"));
			ui.tabWidget->show();
		}
		else
		{
			QMessageBox::critical(0, qApp->tr("Ошибка"),tr("По данному запросу ничего не найдено!"), QMessageBox::Ok,QMessageBox::NoButton);
			ui.line->clear();
			ui.line->setFocus();
		}
	}
}

void fndIPW::sortList()
{
	/*while (!deviceList.isEmpty())
	{
		QStringList* str = strList.takeFirst();
		deviceList->clear();
		delete str;
	}
	for(int i=0;i<deviceList.size();i++)
	{
		QStringList* str=new QStringList();
		strList.append(str);
		for(int j=0;j<deviceList.size();j++)
		{
				strList.at(i)->append(getNametoInt(groupGList.at(i)->deviceList.at(j)->deviceDescr.name));
			}
			strList.at(i)->sort();
		}
		
		for(int i=0;i<strList.size();i++)
		{
			tl = new GrList;
			tempList.append(tl);
			for(int j=0;j<strList.at(i)->size();j++)
			{
				
				for(int k=0;k<groupGList.at(i)->deviceList.size();k++)
				{
					if(strList.at(i)->at(j).contains(getNametoInt(groupGList.at(i)->deviceList.at(k)->deviceDescr.name)))
					{
						tempList.at(i)->deviceList.append(groupGList.at(i)->deviceList.at(k));
						break;
					}
				}
			}
		}
		
		for(int i=0;i<tempList.size();i++)
		{
			groupGList.at(i)->deviceList.clear();
			groupGList.at(i)->deviceList=tempList.at(i)->deviceList;
		}*/
}

void fndIPW::keyPressEvent(QKeyEvent *event){
	if (event->key() == Qt::Key_Escape)
	{
		hide();
	}
	//int n=event->key();
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		on_pb_find_clicked();
	}
	
}

QString fndIPW::getNametoInt(QString str)
{
	uint rez=-1;
	
	QString s;	
	int w=-1;
	for(int y=0;y<str.size();y++)
	{
		rez=str.at(y).unicode();
		if(rez-48>=0 && rez-48<=9)
		{
			s.append(rez);
		}
	}
	if(!s.isEmpty())
		return s;
	else
		return str;
}

void fndIPW::closeEvent(QCloseEvent *event)
{
	switch(parent->currMode)
	{
		case MODE_GG:
			parent->mode_sort=0;
			break;
		case MODE_GA:
			parent->mode_sort=1;
			break;
		case MODE_STATE:
			parent->mode_sort=2;
			break;
		case MODE_COUNT:
			parent->mode_sort=3;
			break;
	}
	parent->updateAfterConfig();
    event->accept();   
}
