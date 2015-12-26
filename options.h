#ifndef OPTIONS_H
#define OPTIONS_H

#include <QtWidgets/QDialog>
#include "ui_options.h"
#include "datatypes.h"


class monitor;

class options : public QDialog
{
	Q_OBJECT

public:
	options(QDialog *parent=0);
	~options();

	bool asc_desc;
	monitor* parent;
	void iShow();
	void getOptions();

	void setpParent(monitor* prt);

private:
	Ui::options ui;

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	

private slots:
	void onSortAsc();
	void onSortDesc();
	void onSortChg();
	void on_pb_close_clicked();
	void on_pb_save_clicked();
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif // OPTIONS_H
