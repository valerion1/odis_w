#ifndef PRBAR_H
#define PRBAR_H

#include <QtWidgets/QWidget>
#include "ui_prbar.h"
#include "terminalsedit.h"

class TerminalsEdit;

class PrBar : public QWidget
{
	Q_OBJECT

public:
	PrBar(QWidget *parent = 0);
	PrBar(QWidget *parent, QRect rect);
	~PrBar();
	void iShow();
	void setVal(int in_val);
	void setMaxVal(int in_val);
	void setBarName(QString in_str);
private:
	Ui::PrBarClass ui;
	TerminalsEdit* prt;
};

#endif // PRBAR_H
