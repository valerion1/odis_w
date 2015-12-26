#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QtWidgets/QWidget>
#include "ui_loginwindow.h"
#include "monitor.h"

class monitor;

class LoginWindow : public QWidget
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = 0);
	~LoginWindow();
	monitor* pParent;
	
private:
	Ui::LoginWindowClass ui;
	void keyPressEvent(QKeyEvent *event);
	

private slots:
	void on_pbCancel_clicked();
	void on_pbOK_clicked();

signals:
	void changeRole();
};

#endif // LOGINWINDOW_H
