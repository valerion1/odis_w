#ifndef DEVICEBUTTON_H
#define DEVICEBUTTON_H

#include <QtWidgets/QWidget>
#include "ui_devicebutton.h"
#include "datatypes.h"

class QImage;

class DeviceButton : public QWidget
{
	Q_OBJECT

public:
	DeviceButton(QWidget *parent = 0);
	~DeviceButton();
	int index;
	IPObject *master;
	void setFace(QImage *in_image);
	void setName(QString in_name);
	void setDescr(QString in_hint);
	void setIndex(IPObject *in_master);
	void setDetails(QImage *in_details);

private:
	Ui::DeviceButtonClass ui;
	QImage face;
	QImage *details;

//private slots:
	//void mousePressEvent(QMouseEvent * event);
	//void paintEvent(QPaintEvent *);

protected:
	void mousePressEvent(QMouseEvent * event);

signals:
	void deviceClicked(IPObject * master);
};

#endif // DEVICEBUTTON_H

