#ifndef MORE_COUNTER_H
#define MORE_COUNTER_H

#include <QtWidgets/QDialog>
#include "ui_more_counter.h"
#include "ipwindow.h"
#include "datatypes.h"


class IPWindow;

typedef struct more_count
{
	QString name;
	QString counter_1_text;
	QString counter_2_text;
	QString counter_1_rm_text;
	QString counter_2_rm_text;
	int type;
	int number;
	int pres;

}more_count;



class more_counter : public QDialog
{
	Q_OBJECT

public:
	more_counter(QDialog *parent = 0);
	~more_counter();

public:
	void iShow();
	void setpParent(IPWindow* prt);
	void setName(QString name);

private:
	Ui::more_counter ui;
	
	IPWindow* parent;
	more_count* list;
	
	void GetData();
	void keyPressEvent(QKeyEvent *event);
		
private slots:
	void on_pb_close_clicked();
	
};

#endif // MORE_COUNTER_H
