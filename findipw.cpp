#include "findipw.h"
#include "monitor.h"
#include "basesettings.h"
#include "ipwindow.h"
#include <QMessageBox>
#include <QCompleter>
#include <QRegExp>



FindIPW::FindIPW(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
	connect(ui.cb_text,SIGNAL(textChanged(QString)),this,SLOT(textChg(QString)));
	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setChg(int)));
	connect(ui.comboBox,SIGNAL(activated(int)),this,SLOT(ActivChg(int)));
	connect(ui.pb_Close, SIGNAL(clicked()),this, SLOT(close()));

	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|Qt::WindowSystemMenuHint|Qt::WindowTitleHint);
	this->setWindowTitle(tr("Поиск ШУНО"));
   	this->setFixedSize(320,200);

	list = new QStringList();
	list->append(tr("Имя Шкафа"));
	list->append(tr("Адрес установки"));
	index=true;
	ipobj=NULL;
	ui.comboBox->addItem(list->at(0));
	ui.comboBox->addItem(list->at(1));	
	ui.cb_text->setFocus();
	completer=NULL;
	key_view=false;
}

FindIPW::~FindIPW()
{

}
void FindIPW::setpParent(monitor* prt)
{
	parent=prt;
}
void FindIPW::setChg(int pos)
{
	ui.cb_text->clear();
	if(pos==0)
	{
		ui.lb_text->setText(tr("Имя Шкафа:"));
	}
	else
	{
		ui.lb_text->setText(tr("Адрес установки:"));
	}
	
	index=pos;
}
void FindIPW::setIndx(bool indx)
{
	index=indx;
}
void FindIPW::iShow()
{
	sortlist.clear();
	for(int i=0;i<ui.cb_text->count();i++)
		ui.cb_text->removeItem(i);
	show();
	LoadCB(index);
}

void FindIPW::textChg(QString str)
{

}
void FindIPW::ActivChg(int pos)
{
	LoadCB(pos);
}
IPObject* FindIPW::FindMaster(QString tmp)
{
	bool key=false;
	int pos=-1;
	switch(index)
	{
	case 0:
		for(int i=0;i<parent->deviceList.count();i++)
		{
			if(parent->deviceList.at(i)->deviceDescr.name==tmp)
			{
				key=true;
				pos=i;
				break;
			}
		}
		break;
	case 1:

		for(int i=0;i<parent->deviceList.count();i++)
		{
			if(parent->deviceList.at(i)->deviceDescr.adress==tmp)
			{
				key=true;
				pos=i;
				break;
			}
		}
		break;
	}

	if(!key)
	{
		QMessageBox::warning(NULL,tr("Ошибка"),tr("Данный терминал отсутствует в списке!"));
		return NULL;
	}
	else
		return parent->deviceList.at(pos);
	
	
}
void FindIPW::on_pb_Find_clicked()
{
	QString str;
	str=ui.cb_text->currentText();
	if(!str.isEmpty())
	{
		ipobj=FindMaster(str);
		if(ipobj)
		{
			BaseSettings *bs = BaseSettings::createInstance();
			bs->ipW->iShow(ipobj);
			bs->freeInst();
			this->close();
		}
		else
		{
			ui.cb_text->setFocus();
			for(int i=0;i<ui.cb_text->count();i++)
				ui.cb_text->removeItem(i);
			sortlist.clear();
		}
	}
	
}

void FindIPW::LoadCB(int ind)
{
	
	for(int i=0;i<ui.cb_text->count();i++)
		ui.cb_text->removeItem(i);
	
	sortlist.clear();
	if(ind==0)
	{
		for(int i=0;i<parent->deviceList.count();i++)
			sortlist<<parent->deviceList.at(i)->deviceDescr.name;
	}
	else
	{
		for(int i=0;i<parent->deviceList.count();i++)
			sortlist<<parent->deviceList.at(i)->deviceDescr.adress;
	}
	if(completer)
		delete completer;
	completer = new QCompleter(sortlist, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	ui.cb_text->setCompleter(completer);
	
	ui.cb_text->setFocus();	
}