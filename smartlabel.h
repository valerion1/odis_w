#ifndef SMARTLABEL_H
#define SMARTLABEL_H

#include <QtWidgets/QLabel>

class SmartLabel : public QLabel
{
	Q_OBJECT

public:
	SmartLabel(QWidget *parent);
	~SmartLabel();

private:
	void mouseDoubleClickEvent ( QMouseEvent * event );
signals:
	void doubleClickFound();
};

#endif // SMARTLABEL_H
