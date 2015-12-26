#include "aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QString aboutText;
	aboutText.append(tr("Программа Монитор\n"));
	aboutText.append(tr("  Версия 3.0.2\n"));
	aboutText.append(tr(" \n"));
	aboutText.append(tr("Часть системы управления освещением СПРУТ\n"));
	aboutText.append(tr("\n"));
	aboutText.append(tr("Автор:\n"));
	aboutText.append(tr("  НПКЦ \"Одис-W\", Одесса\n"));
	aboutText.append(tr("  odisw@ukr.net\n"));
	aboutText.append(tr("  (+38048) 728-07-57\n"));
	aboutText.append(tr(" \n"));
	aboutText.append(tr("\n\nrealMinutes(Delta)+msk!0\n"));
	ui.label->setText(aboutText);
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

AboutWindow::~AboutWindow()
{

}


void AboutWindow::on_pbOK_clicked()
{
	hide();
}