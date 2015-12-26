#ifndef LAVELHISTORY_H
#define LAVELHISTORY_H

#include <QtWidgets/QDialog>
#include "ui_lavelhistory.h"
#include "ipwindow.h"
#include "datatypes.h"

class IPWindow;

class lavelhistory : public QDialog
{
	Q_OBJECT

public:
	lavelhistory(QDialog *parent=0);
	~lavelhistory();
	IPWindow* parent;
	void setpParent(IPWindow* prt);
	void iShow();
	QString phone,name;
	
private:
	Ui::lavelhistory ui;
	

	
	

private slots:
	void on_pushButton_clicked();
};

#endif // LAVELHISTORY_H
