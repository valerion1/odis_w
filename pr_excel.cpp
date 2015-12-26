#include "pr_excel.h"
#include "terminalsedit.h"
#include "datatypes.h"
#include "ipobject.h"

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>

PR_EXCEL::PR_EXCEL(QObject *parent)
	: QObject(parent)
{
	excel=NULL;workbooks=NULL;
	workbook=NULL;sheets=NULL;
	StatSheet=NULL;
	count=0;
	pr_count=0;
	key_view=false;
	data_time.clear();
	file_name.clear();
	
}
PR_EXCEL::PR_EXCEL(QObject *parent,QString ptr_filename,QRect ptr_rect,QString ptr_title)
{
	key_view=false;
	//progress->setBarName(tr("¬ыполн€етс€ EXCEL скрипт..."));

	title.append(ptr_title);
	rect=ptr_rect;
	excel=NULL;
	workbooks=NULL;
	workbook=NULL;
	sheets=NULL;
	StatSheet=NULL;
	count=0;
	pr_count=0;
	key_start=true;
	file=NULL;
	QString str,str1;
	str.clear();
	data_time.clear();
	file_name.clear();

	filename.clear();
	filename=QCoreApplication::applicationDirPath();
	filename=filename+"/Default Files";
	str=filename;
	str1=filename+"/default.xls";
	//str=str+qApp->tr("/PAR_")+QDateTime::currentDateTime().toString("dd.MM.yyyy")+".xls";
	str=QCoreApplication::applicationDirPath()+"/list_teminal.xls";
	QFile::copy(str1,str);
	filename.clear();
	filename=str;
}
PR_EXCEL::~PR_EXCEL()
{
	if(progress)
		delete progress;
	ClearExcel();
}

void PR_EXCEL::setParent(TerminalsEdit* ptr)
{
	parent=(TerminalsEdit*)ptr;
}

void PR_EXCEL::PrintData()
{	
	if(Open_Excel(filename))
	{
		    /*QFile outfile("excel.html");
			QTextStream out( &outfile );
			outfile.open(QIODevice::WriteOnly);
			QString docu = excel->generateDocumentation();
			out << docu;
			outfile.close();*/

		progress = new PrBar(parent,rect);
		progress->setBarName(title);
		progress->setMaxVal(parent->deviceList->size());
		progress->setVal(0);
		progress->iShow();
		PrintHeader();
		//ClearExcel();
		Print();
		//PrintEndFile();
	}
	progress->hide();
	if(key_view)
		excel->dynamicCall( "SetVisible(bool)", TRUE ); //делаем его видимым
}

//-----------------------------servise--------------------------------------------//

void PR_EXCEL::Print()
{
	QAxObject *range;
	QAxObject *Cell1,*Cell2;
	
	int row=2;
	int col=1;
	int numRows=parent->deviceList->size();
	int numCols=3;
	
	Cell1 = StatSheet->querySubObject("Cells(QVariant&,QVariant&)", row, col);
	// получение указател€ на правую нижнюю €чейку [row + numRows - 1][col + numCols - 1] ((!) numRows>=1,numCols>=1)
	Cell2 = StatSheet->querySubObject("Cells(QVariant&,QVariant&)", row + numRows - 1, col + numCols - 1);
	//получение указател€ на целевую область
	range = StatSheet->querySubObject("Range(const QVariant&,const QVariant&)", Cell1->asVariant(), Cell2->asVariant() );
	// список €чеек строки
	QList<QVariant> cellsList;
	// список строк
	QList<QVariant> rowsList;
	for (int j = 0; j < numRows; j++)
	{
		cellsList.clear();
		for (int k = 0; k< numCols; k++)
		{
			cellsList << parent->deviceList->at(j)->deviceDescr.name 
				<< parent->deviceList->at(j)->deviceDescr.phone.right(11)
				<< parent->deviceList->at(j)->deviceDescr.dev_hard_number;
		}
		// в список строк добавл€етс€ QVariant каждой строки
		rowsList << QVariant(cellsList);
		progress->setVal(j);
	}
	// собственно вывод
	range->setProperty("Value", QVariant(rowsList) );
	
	// освобождение пам€ти
	delete range;
	delete Cell1;
	delete Cell2;
}

void PR_EXCEL::Format(QString rng)
{
	QAxObject *range;
	QAxObject *border;
	QAxObject *razmer; 
	QAxObject *pFont;

	//получаю указатель
	range = StatSheet->querySubObject( "Range(const QVariant&)",QVariant(rng));
	range->dynamicCall("Select()");
	//выравнивание в €чейке
	range->dynamicCall("HorizontalAlignment",-4108);
	range->dynamicCall("VerticalAlignment",-4108);
	//границы €чейки

	border = range->querySubObject("Borders(xlEdgeLeft)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);
		
	border = range->querySubObject("Borders(xlEdgeRight)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);
		
	border = range->querySubObject("Borders(xlEdgeTop)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);
		
	border = range->querySubObject("Borders(xlEdgeBottom)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);

	border = range->querySubObject("Borders(xlInsideVertical)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);

	border = range->querySubObject("Borders(xlInsideHorizontal)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);

	pFont = range->querySubObject("Font"); 
	pFont->setProperty("Size", 10); //задаем размер шрифта
	pFont->setProperty("Bold", false); //делаем его полужирным ...
	pFont->setProperty("Italic", false);
	pFont->setProperty("Name", "Times New Roman");
}
	

void PR_EXCEL::PrintHeader()
{

	QAxObject *range;
	QAxObject *border;
	QAxObject *Cell1,*Cell2;

	int start=2;
	QString rng,str;
	str.clear();
	
	int row=1;
	int col=1;
	int numRows=1;
	int numCols=3;


	//»м€ терминала
	str.clear();
	rng.clear();
	rng=rng+"A"+QString::number(row,10)+":A"+QString::number(numRows,10);
	str.append(qApp->tr("»м€ терминала"));
	range = StatSheet->querySubObject( "Range(const QVariant&)",QVariant(rng));
	FormatHeader(rng,15);
	range->dynamicCall("Select()");
	range->dynamicCall( "SetValue(const QVariant&)", QVariant(str));
			

	//“елефон
	str.clear();
	rng.clear();
	rng=rng+"B"+QString::number(row,10)+":B"+QString::number(numRows,10);
	str.append(qApp->tr("“елефон"));
	range = StatSheet->querySubObject( "Range(const QVariant&)",QVariant(rng));
	FormatHeader(rng,15);
	range->dynamicCall("Select()");
	range->dynamicCall( "SetValue(const QVariant&)", QVariant(str));
		

	//»нвентарный номер
	str.clear();
	rng.clear();
	rng=rng+"C"+QString::number(row,10)+":C"+QString::number(numRows,10);
	str.append(qApp->tr("»нвентарный номер"));
	range = StatSheet->querySubObject( "Range(const QVariant&)",QVariant(rng));
	FormatHeader(rng,25);
	range->dynamicCall("Select()");
	range->dynamicCall( "SetValue(const QVariant&)", QVariant(str));
			
	rng.clear();
	rng=rng+"A"+QString::number(start,10)+":C"+QString::number(start+parent->deviceList->size()-1,10);
	Format(rng);
}

void PR_EXCEL::FormatHeader(QString rng,int widght)
{
	QAxObject *range;
	QAxObject *border;
	QAxObject *razmer; 
	QAxObject *pFont;

	//получаю указатель
	range = StatSheet->querySubObject( "Range(const QVariant&)",QVariant(rng));
	range->dynamicCall("Select()");
	//устанавливаю свойство объединени€.
	range->dynamicCall("MergeCells",TRUE);
	//устанавливаю свойство разрешени€ переноса
	range->dynamicCall("WrapText",TRUE);	
	//выравнивание в €чейке
	range->dynamicCall("HorizontalAlignment",-4108);
	range->dynamicCall("VerticalAlignment",-4108);

	//ширина €чейки
	razmer = range->querySubObject("Columns"); 
	razmer->setProperty("ColumnWidth",widght); 

	//границы €чейки

	border = range->querySubObject("Borders(xlEdgeLeft)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);
		
	border = range->querySubObject("Borders(xlEdgeRight)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);
		
	border = range->querySubObject("Borders(xlEdgeTop)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);
		
	border = range->querySubObject("Borders(xlEdgeBottom)");
	border->setProperty("LineStyle",1); 
	border->setProperty("Weight",3);

	pFont = range->querySubObject("Font"); 
	pFont->setProperty("Size", 10); //задаем размер шрифта
	pFont->setProperty("Bold", true); //делаем его полужирным ...
	pFont->setProperty("Italic", false);
	pFont->setProperty("Name", "Times New Roman");
}

bool PR_EXCEL::Open_Excel(QString path)
{
		
	if(StatSheet)
	{
		delete StatSheet;
		StatSheet=NULL;
	}
	if(sheets)
	{
		delete sheets;
		sheets=NULL;
	}
	if(workbook)
	{
		delete workbook;
		workbook=NULL;
	}
	if(workbooks)
	{
		delete workbooks;
		workbooks=NULL;
	}	
	if(excel)
	{
		delete excel;
		excel=NULL;
	}
	if(excel = new QAxObject( "Excel.Application",this))
	{
		workbooks = excel->querySubObject( "Workbooks" );//получаем указатель на список книг
		workbook = workbooks->querySubObject( "Open(const QString&)", path ); // открываем файл с диска с уже созданным документом Excel и получаем указатель на книгу
		sheets = workbook->querySubObject( "Sheets" ); //получаем указатель список листов
		StatSheet = sheets->querySubObject( "Item(const QVariant&)", QVariant(qApp->tr("Ћист1")) ); // получаем указатель на лист с названием УstatФ
		StatSheet->dynamicCall( "Select()" );//на вс€кий случай выбираем этот лист Ц это делает его видимым
				
		if(StatSheet)
		{
			ClearPages();
			QAxObject *pPageSetup = StatSheet->querySubObject("PageSetup");
			if (pPageSetup) 
			{
				pPageSetup->setProperty("LeftMargin", 46);
			    pPageSetup->setProperty("RightMargin", 46);
				pPageSetup->setProperty("TopMargin", 36);
				pPageSetup->setProperty("BottomMargin", 36);
				pPageSetup->setProperty("HeaderMargin", 37);
				pPageSetup->setProperty("FooterMargin", 37);
				pPageSetup->setProperty("Orientation", 1);
			}
			else
				QMessageBox::critical(0, qApp->tr("ќшибка системы"),
						qApp->tr("2"), QMessageBox::Ok,
						QMessageBox::NoButton);
		}
		else
			QMessageBox::critical(0, qApp->tr("ќшибка системы"),
					qApp->tr("1"), QMessageBox::Ok,
					QMessageBox::NoButton);
		return true;
	}
	else
	{
			QMessageBox::critical(0, qApp->tr("ќшибка системы"),
					qApp->tr("ќтсутствует ѕќ \"MS Office\""), QMessageBox::Ok,
					QMessageBox::NoButton);
	}
	return false;
}
void PR_EXCEL::ClearPages()
{
	// получение указател€ на левую верхнюю €чейку [row][col] ((!)нумераци€ с единицы)
	int row=1;
	int col=1;
	int numRows=1500;
	int numCols=10;

	QAxObject* Cell1 = StatSheet->querySubObject("Cells(QVariant&,QVariant&)", row, col);
	// получение указател€ на правую нижнюю €чейку [row + numRows - 1][col + numCols - 1] ((!) numRows>=1,numCols>=1)
	QAxObject* Cell2 = StatSheet->querySubObject("Cells(QVariant&,QVariant&)", row + numRows - 1, col + numCols - 1);
	// получение указател€ на целевую область
	QAxObject* range = StatSheet->querySubObject("Range(const QVariant&,const QVariant&)", Cell1->asVariant(), Cell2->asVariant() );
	range->dynamicCall( "Clear()" );

	// освобождение пам€ти
	delete range;
	delete Cell1;
	delete Cell2;
}
void PR_EXCEL::ClearExcel()
{
	if(StatSheet)
	{
		delete StatSheet;
		StatSheet=NULL;
	}
	if(sheets)
	{
		delete sheets;
		sheets=NULL;
	}
	if(workbook)
	{
		//workbook->dynamicCall("Save()",QVariant(filename));
		delete workbook;
		workbook=NULL;
	}
	if(workbooks)
	{
		//if(!key_view)
		//	workbooks->dynamicCall("Close()");
		delete workbooks;
		workbooks=NULL;
	}	
	if(excel)
	{
		delete excel;
		excel=NULL;
	}
}
