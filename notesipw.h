#ifndef NOTESIPW_H
#define NOTESIPW_H

#include <QtWidgets/QDialog>
#include "ui_notes.h"
#include "datatypes.h"

class notesIPW : public QDialog
{
	Q_OBJECT

public:
	notesIPW(QDialog *parent=0);
	~notesIPW();
	void iShow();
	void setNotes(QString);
	void setTitle(QString);

private:
	Ui::notesIPW ui;
	
	
};

#endif // NOTESIPW_H
