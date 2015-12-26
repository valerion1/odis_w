#ifndef GROUPFINDWIDGET_H
#define GROUPFINDWIDGET_H

#include <QtWidgets/QWidget>
#include "datatypes.h"
#include "ui_widgetfind.h"

class FlowLayout;
class DeviceButton;
class QScrollArea;

class groupfindwidget : public QWidget
{
	Q_OBJECT

public:
	groupfindwidget(QWidget *parent);
	~groupfindwidget();
	void AddDevice(DeviceButton *in_b);
	void DelDevice(DeviceButton *in_b);

	

private:
	Ui::GroupFindClass ui;
	FlowLayout *FL;
	QScrollArea *scrollArea;
	
};

#endif // GROUPFINDWIDGET_H
