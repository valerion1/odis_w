#ifndef QWEST_DLG_H
#define QWEST_DLG_H

#include <QDialog>
#include "ui_qwestdialog.h"

class Qwest_dlg : public QDialog
{
	Q_OBJECT

public:
	Qwest_dlg(QDialog *parent);
	~Qwest_dlg();

private:
	Ui::qwestdialog ui;
	
};

#endif // QWEST_DLG_H
