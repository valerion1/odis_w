#ifndef FIND_TERMINAL_H
#define FIND_TERMINAL_H

#include <QtWidgets/QDialog>
#include "ui_find_terminal.h"
#include "terminalsedit.h"


class TerminalsEdit;

class Find_terminal : public QDialog
{
	Q_OBJECT

public:
	Find_terminal(QDialog *parent=0);
	~Find_terminal();

	void setParent(TerminalsEdit*);
	void iShow();
	void setIndx(bool indx = true);
		
private:
	Ui::Find_terminal ui;
	TerminalsEdit* parent;
	QStringList* list;
	QRegExp shablon;
	int index;
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	QString getNametoInt(QString);
	void buildGroupList(QString);
	

private slots:
	void on_pb_close_clicked();
	void on_pb_find_clicked();
	void setChg(int);
};

#endif // FIND_TERMINAL_H
