#include "find_terminal.h"
#include "terminalsedit.h"

#include <QKeyEvent>
#include <QCloseEvent>
#include <QRegExp>
#include <QtWidgets/QMessageBox>

Find_terminal::Find_terminal(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	
	//???????????????????????connect(this, SIGNAL(clicked()),this, SLOT(close()));
	connect(ui.pb_close, SIGNAL(clicked()),this, SLOT(close()));
	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setChg(int)));

	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|/*Qt::WindowSystemMenuHint|*/Qt::WindowTitleHint);
	this->setWindowTitle(tr("Поиск терминала"));
   	this->setFixedSize(350,200);

	list=NULL;
	list = new QStringList();
	list->append(tr("Имя ШУНО"));
	list->append(tr("Инвертарный номер"));
	list->append(tr("Номер телефона ШУНО"));
	index=true;
	ui.comboBox->addItem(list->at(0));
	ui.comboBox->addItem(list->at(1));
	ui.comboBox->addItem(list->at(2));
	
	
}

Find_terminal::~Find_terminal()
{
	if(list) delete list;
}
void Find_terminal::setIndx(bool indx)
{
	index=indx;
}
void Find_terminal::setParent(TerminalsEdit* ptr)
{
	parent=(TerminalsEdit*) ptr;
}
void Find_terminal::iShow()
{
	ui.lineEdit->clear();
	show();
	ui.lineEdit->setFocus();
}
void Find_terminal::setChg(int pos)
{
	ui.lineEdit->clear();
	switch(pos)
	{
	case 0:
		{
			ui.lb_text->setText(tr("Имя ШУНО:"));
			break;
		}
	case 1:
		{
			ui.lb_text->setText(tr("Инвертарный номер:"));
			break;
		}
	case 2:
		{
			ui.lb_text->setText(tr("Номер телефона ШУНО:"));
			break;
		}
	}
	index=pos;
	ui.lineEdit->setFocus();
}
void Find_terminal::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		hide();
	}
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		on_pb_find_clicked();
	}
	
}
void Find_terminal::closeEvent(QCloseEvent *event)
{
	hide();
    event->accept();   
   
}
QString Find_terminal::getNametoInt(QString str)
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

void Find_terminal::buildGroupList(QString str)
{
	bool key_serch=false;
			
	if(str.isEmpty())
	{
		QMessageBox::critical(0, qApp->tr("Ошибка"),tr("По данному запросу ничего не найдено!"), QMessageBox::Ok,QMessageBox::NoButton);
		ui.lineEdit->clear();
		ui.lineEdit->setFocus();
	}
	else
	{
		shablon.setPatternSyntax(QRegExp::Wildcard);
		
		
		switch(index)
		{
		case 0:
			{
				shablon.setPattern(str);
				for(int i=0;i<parent->ui.tableWidget->rowCount();i++)
				{
					//QString s=getNametoInt(parent->ui.tableWidget->item(i,1)->text().toLower());
					if (shablon.exactMatch(getNametoInt(parent->ui.tableWidget->item(i,1)->text().toLower())))
					{	
						parent->position_view=i;
						parent->position_key=1;
						key_serch=true;
						break;
					}
				}
				break;
			}
		case 1:
			{
				QString tmp="*"+str+"*";
				shablon.setPattern(tmp);
				for(int i=0;i<parent->ui.tableWidget->rowCount();i++)
				{
					//QString s=getNametoInt(parent->ui.tableWidget->item(i,3)->text().toLower());
					if (shablon.exactMatch(getNametoInt(parent->ui.tableWidget->item(i,3)->text().toLower())))
					{	
						parent->position_view=i;
						parent->position_key=3;
						key_serch=true;
						break;
					}
				}
				break;
			}
		case 2:
			{
				QString tmp="*"+str+"*";
				shablon.setPattern(tmp);
				for(int i=0;i<parent->ui.tableWidget->rowCount();i++)
				{
					//QString s=getNametoInt(parent->ui.tableWidget->item(i,2)->text().toLower());
					if (shablon.exactMatch(getNametoInt(parent->ui.tableWidget->item(i,2)->text().toLower())))
					{	
						parent->position_view=i;
						parent->position_key=2;
						key_serch=true;
						break;
					}
				}
				break;
			}
		}
		if(key_serch)
		{
			parent->setPositionView();
			on_pb_close_clicked();
		}
		else
		{
			QMessageBox::critical(0, qApp->tr("Ошибка"),tr("По данному запросу ничего не найдено!"), QMessageBox::Ok,QMessageBox::NoButton);
			ui.lineEdit->clear();
			ui.lineEdit->setFocus();
		}
	}
}

void Find_terminal::on_pb_find_clicked()
{
	buildGroupList(ui.lineEdit->text());
}

void Find_terminal::on_pb_close_clicked()
{
	hide();
}
