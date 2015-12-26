#include "loginwindow.h"
#include "basesettings.h"
#include "monitor.h"

#include <QtWidgets/QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QKeyEvent>


LoginWindow::LoginWindow(QWidget *parent)
	: QWidget(parent)
{
	pParent=(monitor*)parent;
	ui.setupUi(this);
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
	ui.leLogin->setFocus();
}

LoginWindow::~LoginWindow()
{

}


void LoginWindow::on_pbOK_clicked()
{
	QString loginName = ui.leLogin->text();
	QString password = ui.lePassword->text();
	BaseSettings *bs = BaseSettings::createInstance();
	QSqlQuery tq;
	if (ui.checkBox->isChecked()){
		loginName = bs->settings->value("Connection/default_user","user").toString();
		password = "";
	}
	QString str="select user_role from user where name='"+loginName+"' and passwd=PASSWORD('"+password+"')";
	/*tq.prepare("select user_role from user where name=:name and passwd=PASSWORD(:passwd)");
	tq.bindValue(":name",loginName);
	tq.bindValue(":passwd",password);*/
	if (!tq.exec(str)){
		QSqlError err = tq.lastError();
		QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
			err.text(), QMessageBox::Cancel,
			QMessageBox::NoButton);
	}else{
		if (tq.next()){
			int userRole = tq.value(tq.record().indexOf("user_role")).toUInt();
			bs->accessLevel = userRole;
			if(userRole>1)
			{
				pParent->informationWindow->key_login=true;
				pParent->key_option=true;
				pParent->key_login=true;
			}
			else
			{
				pParent->informationWindow->key_login=false;
				pParent->key_option=false;
				pParent->key_login=false;
			}
			/*if(userRole>2)
			{
				pParent->key_login=true;
				pParent->key_option=true;
			}
			else
			{
				pParent->key_login=false;
				pParent->key_option=false;
			}*/
			emit changeRole();
			hide();
			ui.leLogin->clear();
			ui.lePassword->clear();
		}else{
			QMessageBox::critical(0, qApp->tr("Ошибка"),
				tr("Неверный логин или пароль"), QMessageBox::Cancel,
				QMessageBox::NoButton);
		}
	}
	bs->freeInst();
	ui.lePassword->clear();
	ui.leLogin->setFocus();
}

void LoginWindow::on_pbCancel_clicked()
{
	ui.leLogin->clear();
	ui.lePassword->clear();
	hide();
	ui.leLogin->setFocus();
}
void LoginWindow::keyPressEvent(QKeyEvent *event){
	
	if (event->key() == Qt::Key_Escape)
	{
		hide();
	}
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		on_pbOK_clicked();
	}
}
