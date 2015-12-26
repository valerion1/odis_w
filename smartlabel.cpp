#include "smartlabel.h"

SmartLabel::SmartLabel(QWidget *parent)
	: QLabel(parent)
{
    setScaledContents(true) ;
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

SmartLabel::~SmartLabel()
{

}

void SmartLabel::mouseDoubleClickEvent ( QMouseEvent * event ){
	emit doubleClickFound();
}
