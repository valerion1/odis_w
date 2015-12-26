#ifndef PR_EXCEL_H
#define PR_EXCEL_H

#include "datatypes.h"
#include "terminalsedit.h"
#include "datatypes.h"

#include "prbar.h"
#include <QObject>
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QFile>

class PrBar;
class TerminalsEdit;


class PR_EXCEL : public QObject
{
	Q_OBJECT

public:
	PR_EXCEL(QObject *parent);
	PR_EXCEL(QObject *parent,QString,QRect,QString);
	~PR_EXCEL();
	
	QString data_time;
	QString file_name;
	void PrintData();
	bool key_view;
	void setParent(TerminalsEdit*);
	
private:
	TerminalsEdit* parent;
	QFile* file;
	QAxObject* excel;
	QAxObject *workbooks,*workbook,*sheets,*StatSheet;
	PrBar *progress;
	QRect rect;
	QString title;
	int pr_count;
	QString filename;
	int count;
	bool key_start;

	bool Open_Excel(QString);
        // old void PR_EXCEL::ClearExcel();
        void ClearExcel();
	void Print();
	void PrintHeader();
	void Format(QString);
	void FormatHeader(QString,int);
	void ClearPages();
	
};

#endif // PR_EXCEL_H
