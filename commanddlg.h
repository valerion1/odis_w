#ifndef COMMANDDLG_H
#define COMMANDDLG_H

#include <QDialog>
#include "ui_commanddlg.h"
#include "ipwindow.h"
#include "datatypes.h"

class IPWindow;

class CommandDialog : public QDialog
{
	Q_OBJECT

public:
	CommandDialog(QDialog *parent=0);
	~CommandDialog();
	void iShow();
	void setParent(IPWindow *);
private:
	Ui::commanddlg ui;

	IPWindow* parent;

	

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
signals:
	void ToServer(int);
private slots:
	void on_pushButton_2_clicked();
	void on_pushButton_clicked();
	void on_kontaktors();
	void on_servise();
	//void on_anycommands();
	
};
#endif