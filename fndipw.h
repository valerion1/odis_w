#ifndef FNDIPW_H
#define FNDIPW_H

#include <QtWidgets/QDialog>
#include "ui_fndIPW.h"
#include "monitor.h"
#include "ipwindow.h"
#include "datatypes.h"
#include "groupfindwidget.h"

class monitor;
class IPWindow;

class fndIPW : public QDialog
{
	Q_OBJECT

public:
	fndIPW(QDialog *parent = 0);
	~fndIPW();

public:
	void iShow();
	void setIndx(bool indx = true);
	void setpParent(monitor* prt);

private:
	Ui::fndIPW ui;
	int index;
	monitor* parent;
	QStringList* list;
	IPObject* ipobj;
	QRegExp shablon;
	DevList deviceList;
	groupfindwidget* group;
	
	void buildGroupList(QString);
	void keyPressEvent(QKeyEvent *event);
	void sortList();
	void closeEvent(QCloseEvent *event);
	QString getNametoInt(QString);
	QList <QStringList*> strList;
	
private slots:
	void on_pb_close_clicked();
	void on_pb_find_clicked();
	void setChg(int);
};

#endif // FNDIPW_H
