#include "colorselect.h"
#include "basesettings.h"

#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QColorDialog>
#include <QFileInfo>
#include <QtWidgets/QMessageBox>

ColorSelect::ColorSelect(QWidget *parent)
    : QWidget(parent)
{
    
    ui.setupUi(this);
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
//    connect(ui.tableWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(on_tb_clicked(QModelIndex)));
    
}

ColorSelect::~ColorSelect()
{

}

void ColorSelect::iShow(){

    ui.pbChange->setDisabled(true);
    ui.pbSave->setDisabled(true);

    ui.tableWidget->clear();
    BaseSettings *bs = BaseSettings::createInstance();
    int colorsCount = bs->settings->value("colors/col_count","0").toInt();
    QTableWidgetItem * tI;
    ui.tableWidget->setRowCount(colorsCount);
    ui.tableWidget->setColumnCount(2);
    for (int i=0;i<colorsCount;i++){
        tI = new QTableWidgetItem(bs->settings->value("colors/color_"+QString::number(i,10)+"_text","color").toString());
        tI->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        ui.tableWidget->setItem(i,1,tI);
        tI = new QTableWidgetItem(" ");
        tI->setFlags(Qt::ItemIsEnabled);
        tI->setBackgroundColor(QColor(QRgb(bs->settings->value("colors/color_"+QString::number(i,10)+"_value","0").toUInt())));
        ui.tableWidget->setItem(i,0,tI);
    }
    tI = new QTableWidgetItem(tr("Цвет"));
    ui.tableWidget->setHorizontalHeaderItem(0,tI);
    tI = new QTableWidgetItem(tr("Название элемента"));
    ui.tableWidget->setHorizontalHeaderItem(1,tI);
    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->setColumnWidth(0,40);
    ui.tableWidget->setColumnWidth(1,this->width()-46-140);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
	connect(ui.tableWidget->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(slot_SelectionChanged(const QItemSelection &, const QItemSelection &)));

	//////////////////////////////////////////////////////////////////////////
    QTableWidgetItem* tItem;
    for (int i=0;i<colorsCount;i++)
    {
        tItem=ui.tableWidget->item(i,1);
        tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }

    //////////////////////////////////////////////////////////////////////////
    bs->freeInst();
    //////////////////////////////////////////////////////////////////////////
    show();
}

void ColorSelect::setpParent(monitor* prt)
{
    parent=(monitor*)prt;
}
void ColorSelect::on_pbClose_clicked()
{
    switch(parent->currMode)
    {
        case MODE_GG:
            parent->mode_sort=0;
            break;
        case MODE_GA:
            parent->mode_sort=1;
            break;
        case MODE_STATE:
            parent->mode_sort=2;
            break;
        case MODE_COUNT:
            parent->mode_sort=3;
            break;
    }
    hide();
    ui.tableWidget->clear();
    emit refreshG();
}

void ColorSelect::saveColorInfo(){
    
    QFileInfo info("monitor.ini");

    if(info.isWritable())
    {
        //save
        BaseSettings *bs = BaseSettings::createInstance();
        int colorCount = ui.tableWidget->rowCount();
        for (int i = 0; i<colorCount; i++){
            //
            bs->settings->setValue("colors/color_"+QString::number(i,10)+"_text",ui.tableWidget->item(i,1)->text());
            bs->settings->setValue("colors/color_"+QString::number(i,10)+"_value",ui.tableWidget->item(i,0)->backgroundColor().rgba());
        }
        bs->freeInst();
    }
    else
    {
        QMessageBox::critical(this, tr("Предупреждение системы"),tr("Файл конфигурации защищен от записи!\n  Обратитесь в сервисную службу!")); 
    }
}

void ColorSelect::on_pbSave_clicked()
{
    saveColorInfo();
    ui.pbChange->setDisabled(true);
    ui.pbSave->setDisabled(true);
}

void ColorSelect::on_tableWidget_itemDoubleClicked(QTableWidgetItem* tI){}

void ColorSelect::slot_SelectionChanged(const QItemSelection &, const QItemSelection &)
{
    ui.pbChange->setDisabled(false);
}

void ColorSelect::on_pbChange_clicked()
{
    QModelIndex curIndex = ui.tableWidget->currentIndex();
    int row = curIndex.row();
    QTableWidgetItem* tI=ui.tableWidget->item(row,0);
    if (tI->text() == " ")
    {
        QColor newColor =QColorDialog::getColor(tI->backgroundColor(),NULL);
        if(newColor.isValid())
        {
            tI->setBackgroundColor(newColor);
            ui.pbSave->setDisabled(false);
        }
    }
}

/*void ColorSelect::on_t b_clicked(QModelIndex model)
{
    ui.pbChange->setDisabled(false);
}*/
