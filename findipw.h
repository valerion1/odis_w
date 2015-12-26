#ifndef FINDIPW_H
#define FINDIPW_H

#include <QtWidgets/QDialog>
#include <QRegExp>
#include "ui_FindIPW.h"
#include "monitor.h"
#include "ipwindow.h"

class monitor;
class IPWindow;
class QCompleter;

class FindIPW : public QDialog
{
	Q_OBJECT

public:
	FindIPW(QDialog *parent = 0);
	~FindIPW();


public:
	void setIndx(bool indx = true);
	void iShow();
	void setpParent(monitor* prt);
private:
	Ui::FindIPW ui;
	QCompleter *completer;
	int index;
	monitor* parent;
	QStringList* list;
	IPObject* ipobj;
	QStringList sortlist;
	bool key_view;
	QRegExp shablon;
	
	IPObject* FindMaster(QString);
	void LoadCB(int);
	


private slots:
	void on_pb_Find_clicked();
	void setChg(int);
	void ActivChg(int);
	void textChg(QString);
	
	
	
};

#endif // FINDIPW_H
