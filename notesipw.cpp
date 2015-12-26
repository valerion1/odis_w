#include "notesipw.h"

notesIPW::notesIPW(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Window|Qt::WindowTitleHint);
	this->setFixedSize(300,150);
	connect(ui.pbClose, SIGNAL(clicked()),this, SLOT(close()));
	ui.text->setDisabled(true);
	

}

notesIPW::~notesIPW()
{

}
void notesIPW::iShow()
{
	show();
}
void notesIPW::setNotes(QString str)
{
	ui.text->clear();
	ui.text->setText(str);
}
void notesIPW::setTitle(QString title)
{
	QString str;
	str.append(tr("Примечание для ")+title);
	this->setWindowTitle(str);
}
