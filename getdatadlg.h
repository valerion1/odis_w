#ifndef GETDATADLG_H
#define GETDATADLG_H

#include <QDialog>
#include "ui_getdatadlg.h"
#include "ipwindow.h"
#include "datatypes.h"

class IPWindow;

class GetDataDialog : public QDialog
{
	Q_OBJECT

public:
	GetDataDialog(QDialog *parent=0);
	~GetDataDialog();
	void iShow();
	
	void setParent(IPWindow *);
private:
	Ui::getdatadlg ui;

	IPWindow* parent;

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
signals:
	void ToServer(int);
private slots:
	void on_pb_cancel_clicked();
	void on_pb_ok_clicked();
};
#endif