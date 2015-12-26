#include "prbar.h"
#include "terminalsedit.h"

PrBar::PrBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.progressBar->setValue(0);
	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(
		Qt::Window|
		Qt::MSWindowsFixedSizeDialogHint|//Gives the window a thin dialog border on Windows. 
		//This style is traditionally used for fixed-size dialogs.
		Qt::WindowTitleHint//|//Gives the window a title bar.
		//Qt::WindowStaysOnTopHint //Informs the window system that the window should stay on top of all 
		//other windows. Note that on some window managers on X11 you also have to pass 
		//Qt::X11BypassWindowManagerHint for this flag to work correctly.
		//NOT IMPLEMENT YET - INVISIBLE!!!
		);
	
	
}
PrBar::PrBar(QWidget *parent, QRect rect)
	: QWidget(parent)
{
	prt=(TerminalsEdit*)parent;
	ui.setupUi(this);
	ui.progressBar->setValue(0);
	this->setWindowModality(Qt::NonModal);
	this->setWindowFlags(Qt::Window|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowTitleHint);
	this->setFixedSize(400,60);
	this->setGeometry(rect.left()+rect.width()/4, rect.top()+rect.height()/2-30, 400, 60);
}
PrBar::~PrBar()
{

}
void PrBar::iShow()
{
	show();
	this->setDisabled(true);
}

void PrBar::setVal(int in_val){
	ui.progressBar->setValue(in_val);
	ui.label_txtCount->setText(QString::number(int(in_val)));
}

void PrBar::setMaxVal(int in_val){
	ui.progressBar->setMaximum(in_val);
}

void PrBar::setBarName(QString in_str){
	this->setWindowTitle(in_str);
}