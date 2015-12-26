#include "groupfindwidget.h"
#include <QtWidgets/QListWidgetItem>
#include <QPainter>
#include "basesettings.h"
#include "flowlayout.h"
#include "devicebutton.h"
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QFrame>


groupfindwidget::groupfindwidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	QFrame *frame = new QFrame();
	frame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	frame->setFrameStyle(QFrame::Box|QFrame::Plain);
	FL=NULL;
	FL = new FlowLayout();
	frame->setLayout(FL);

	scrollArea=NULL;
	scrollArea = new QScrollArea();
	scrollArea->setWidget(frame);
	scrollArea->setWidgetResizable(true);
	ui.verticalLayout_2->addWidget(scrollArea);
	ui.pushButton->hide();
}

groupfindwidget::~groupfindwidget()
{
	if(FL)
		delete FL;
	if(scrollArea)
		delete scrollArea;
}
void groupfindwidget::AddDevice(DeviceButton *in_b)
{
	FL->addWidget(in_b);
}

void groupfindwidget::DelDevice(DeviceButton *in_b)
{
	FL->removeWidget(in_b);
}
