#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QtWidgets/QWidget>
#include "ui_aboutwindow.h"

class AboutWindow : public QWidget
{
	Q_OBJECT

public:
	AboutWindow(QWidget *parent = 0);
	~AboutWindow();

private:
	Ui::AboutWindowClass ui;

private slots:
	void on_pbOK_clicked();
};

#endif // ABOUTWINDOW_H
