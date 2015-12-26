#ifndef SERVICETABLERECORD_H
#define SERVICETABLERECORD_H

#include <QtWidgets/QWidget>
#include "ui_servicetablerecord.h"

class ServiceTableRecord : public QWidget
{
	Q_OBJECT

public:
	ServiceTableRecord(QWidget *parent = 0);
	~ServiceTableRecord();
	void iShow(QString tbl_name);
private:
	Ui::ServiceTableRecordClass ui;

private slots:
	void on_pbCalcel_clicked();
	void on_pbAdd_clicked();
signals:
	void RetVal(QString,int);
};

#endif // SERVICETABLERECORD_H
