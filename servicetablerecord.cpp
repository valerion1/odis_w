#include "servicetablerecord.h"
#include <QtWidgets/QMessageBox>
#include <Qt>

ServiceTableRecord::ServiceTableRecord(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.lineEdit,SIGNAL(returnPressed()),ui.pbAdd,SLOT( click () ));
}

ServiceTableRecord::~ServiceTableRecord()
{

}


void ServiceTableRecord::on_pbAdd_clicked()
{
	if (ui.lineEdit->text()!= ""){
		if (ui.lineEdit_2->text()!=""){
            bool Ok = false;
			int intVal = ui.lineEdit_2->text().toInt(&Ok,10);
			if (Ok){
				emit RetVal(ui.lineEdit->text(),intVal);
				hide();
			}else{
				QMessageBox::warning(0, qApp->tr("Добавление записи"),
					tr("Значение не является числом"), QMessageBox::Cancel,
					QMessageBox::NoButton);
			}
		}else{
			QMessageBox::warning(0, qApp->tr("Добавление записи"),
				tr("Введите Значение"), QMessageBox::Cancel,
				QMessageBox::NoButton);
		}
	}else{
		QMessageBox::warning(0, qApp->tr("Добавление записи"),
			tr("Введите Описание"), QMessageBox::Cancel,
			QMessageBox::NoButton);
	}
}

void ServiceTableRecord::iShow(QString tbl_name){
	ui.lineEdit->setText("");
	ui.lineEdit_2->setFocus(Qt::OtherFocusReason);
	ui.lineEdit_2->setText("");
	show();
}

void ServiceTableRecord::on_pbCalcel_clicked()
{
	hide();
}
