#include "groupgedit.h"
#include "basesettings.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QtWidgets/QMessageBox>
#include <QModelIndex>
#include <QtWidgets/QHeaderView>
#include <QSqlField>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QSqlRecord>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QKeyEvent>
#include <QtWidgets/QTableWidgetItem>
#include <QFileInfo>

GroupGEdit::GroupGEdit(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(tr("Редактирование групп по географическому признаку"));
	ui.tableView->setSortingEnabled(TRUE);
    connect(ui.tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sortChanged(int)));
    model = new QSqlTableModel(this,tDB);
	initializeModel(model);
    //connect(ui.tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(on_t b_clicked(QModelIndex)));//этого нет в .ui

	ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.pbDel->setDisabled(true);
}

GroupGEdit::~GroupGEdit()
{
	if (model) delete model;
	saveColorInfo();
}

void GroupGEdit::setParent(monitor* prt)
{
    parent=(monitor*)prt;
}

void GroupGEdit::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) close();
}

void GroupGEdit::initializeModel(QSqlTableModel *temp_model)
{
	temp_model->setTable("groups_g");
	temp_model->setEditStrategy(QSqlTableModel::OnFieldChange);
	temp_model->select();
	//temp_model->setHeaderData(0, Qt::Horizontal, QObject::tr("i"));
	temp_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя"));
	temp_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Описание"));
}

void GroupGEdit::iShow(){
    refresh();
    show();
}

void GroupGEdit::refresh(){
/*    QSqlDatabase tDB = QSqlDatabase::database();
    QSqlTableModel *TModel = NULL;
    if (tDB.isOpen()){
        if (model){
            TModel = model;
        }*/
	model->select();
        //if (model->select()){
            //model->removeColumn(0); индекс не удаляем - а то как потом искать будем
            //model->removeColumn(3);
            //model->removeColumn(3);
            //model->removeColumn(3);
            //model->setHeaderData(0, Qt::Horizontal, QObject::tr("Индекс"));
            //model->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя"));
            //model->setHeaderData(2, Qt::Horizontal, QObject::tr("Описание"));
            //model->setEditStrategy(QSqlTableModel::OnFieldChange);
            ui.tableView->setModel(model);

            //if (TModel) delete TModel;
            ui.tableView->setColumnHidden(0,true);
            ui.tableView->setColumnHidden(3,true);
            ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
            ui.tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

            // old ui.tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
            // old ui.tableView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);

            ui.tableView->resizeColumnsToContents();
            ui.tableView->resizeRowsToContents();
			if (ui.tableView->columnWidth(1)<80) ui.tableView->setColumnWidth(1,80);
            if(parent->sort_g)
                ui.tableView->sortByColumn(1,Qt::AscendingOrder);
            else
                ui.tableView->sortByColumn(1,Qt::DescendingOrder);
            //ui.tableView->show();
        /*}else{
            QSqlError err = tDB.lastError();
            QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
                err.text(), QMessageBox::Cancel,
                QMessageBox::NoButton);
            ui.tableView->setEnabled(FALSE);
        }
    }else{
        QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
            tr("База не открыта"), QMessageBox::Cancel,
            QMessageBox::NoButton);
        ui.tableView->setEnabled(FALSE);
    }*/
	if (model->rowCount()>0)
	{
		ui.pbDel->setDisabled(false);
		ui.tableView->setCurrentIndex(model->index(0,0));//если рефреш после редактирования - оставим курсор где был, иначе поставим на первую позицию
	}
	else ui.pbDel->setDisabled(true);
    adjustSize();
}

void GroupGEdit::on_pbAdd_clicked()
{
    QSqlQuery *tq = new QSqlQuery();
    //QString str="insert into avtonom_groups (name, human_descr) values ("+tr("'Новая группа', ")+tr("'Описание новой группы'")+");";
    /*tq->prepare("insert into groups_g (name, human_descr) values (:name, :human_descr); ");
    tq->bindValue(":name",tr("Новая группа"));
    tq->bindValue(":human_descr",tr("Описание новой группы"));*/
    QString str="insert into groups_g (name, human_descr) values ("+tr("'Новая группа', ")+tr("'Описание новой группы'")+");";
    tq->exec(str);
    delete tq;
	emit refreshG();
    refresh();
    //ui.pbDel->setDisabled(true);
}

void GroupGEdit::on_pbDel_clicked()
{
    QString str;
    int count=0;
    QModelIndex curIndex = ui.tableView->currentIndex();
    int row = curIndex.row();
    QSqlRecord record = model->record(row);
    int UserNameIndex = record.indexOf("rec_id");
    int ls_UserName=record.value(UserNameIndex).toInt();
    QSqlQuery *tq = new QSqlQuery();
    str="select * from device where group_g="+QString::number(ls_UserName,10);
    tq->exec(str);
    if(tq->isActive())
    {
        tq->first();
        while(tq->next())
        {
            count++;
        }
    }
    if(count>0)
    {
        QMessageBox::critical(0, qApp->tr("Предупреждение системы"),
            tr("Для удаления группы необходимо удалить все принадлежащие ей приборы! "), QMessageBox::Cancel,
            QMessageBox::NoButton);
    }
    else
    {
        str="delete from groups_g where rec_id="+QString::number(ls_UserName,10);
        tq->exec(str);
    }
    delete tq;
	emit refreshG();
    refresh();
   // ui.pbDel->setDisabled(true);
}

void GroupGEdit::sortChanged(int logicalIndex)
{
    if(logicalIndex==1)
    {
        if(ui.tableView->horizontalHeader()->sortIndicatorOrder()==Qt::AscendingOrder)
        {
            parent->sort_g=true;
        }
        else
        {
            parent->sort_g=false;
        }
        parent->key_widg=true;
	    emit refreshG();
    }
}

void GroupGEdit::saveColorInfo(){
    
    QFileInfo info("monitor.ini");

    if(info.isWritable())
    {
        
        BaseSettings *bs = BaseSettings::createInstance();
        if(parent->sort_g)
            bs->settings->setValue("sort/sort_g",QString::number(1,10));
        else
            bs->settings->setValue("sort/sort_g",QString::number(0,10));
        
        bs->freeInst();
    }
    else
    {
        QMessageBox::critical(this, tr("Предупреждение системы"),tr("Файл конфигурации защищен от записи!\n  Обратитесь в сервисную службу!")); 
    }
}

void GroupGEdit::on_pbExit_clicked()
{close();}

void GroupGEdit::closeEvent(QCloseEvent *event)
{event->accept();}//или удалять из родителя и обнулять указатель или скрывать не удаляя

