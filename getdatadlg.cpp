#include "getdatadlg.h"
#include "ipwindow.h"
#include "datatypes.h"

#include <QCloseEvent>
#include <QKeyEvent>

GetDataDialog::GetDataDialog(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(this, SIGNAL(clicked()),this, SLOT(close()));
	
	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|/*Qt::WindowSystemMenuHint|*/Qt::WindowTitleHint);
	this->setWindowTitle(tr("Составной запрос"));
   	this->setFixedSize(250,280);

	

}

GetDataDialog::~GetDataDialog()
{

}

void GetDataDialog::iShow()
{

	ui.cb_state->setChecked(false);
	ui.cb_config->setChecked(false);
	ui.cb_delta->setChecked(false);
	ui.cb_time->setChecked(false);
	ui.cb_count->setChecked(false);
	ui.cb_network->setChecked(false);
	ui.cb_pascount->setChecked(false);
	ui.cb_telnumber->setChecked(false);
	ui.cb_gprs->setChecked(false);
	
	show();
}



void GetDataDialog::on_pb_ok_clicked()
{
	QString str;

	parent->COMM_STR.clear();
	str.clear();
	
	if(ui.cb_state->isChecked())
	{
		str.append("ST=?;");
	}
	if(ui.cb_time->isChecked())
	{
		str.append("TM=?;");
	}
	if(ui.cb_delta->isChecked())
	{
		str.append("DL=?;");
	}
	if(ui.cb_config->isChecked())
	{
		str.append("MSK=?;");
	}
	if(ui.cb_network->isChecked())
	{
		str.append("SQY=?;");
	}
	if(ui.cb_count->isChecked())
	{
		str.append("CNT1A=?;CNT1RP=?;CNT1RM=?;CNT2A=?;");
	}
	if(ui.cb_pascount->isChecked())
	{
		//str.append("");
	}
	if(ui.cb_telnumber->isChecked())
	{
		//str.append("");
	}
	if(ui.cb_gprs->isChecked())
	{
		//str.append("");
	}
	
	if(!str.isEmpty())
	{
		parent->COMM_STR.append(str);
		emit ToServer(1);
	}
	hide();
}

void GetDataDialog::on_pb_cancel_clicked()
{
	hide();
}
void GetDataDialog::setParent(IPWindow* prt)
{
	parent=(IPWindow*)prt;
	
}
void GetDataDialog::closeEvent(QCloseEvent *event)
{
	event->accept();   
   
}
void GetDataDialog::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		hide();
	}
}