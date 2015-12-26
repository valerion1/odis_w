#include "groupaedit.h"
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
#include <QFileInfo>
GroupAEdit::GroupAEdit(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(tr("Редактирование пользовательских групп"));
	ui.tableView->setSortingEnabled(TRUE);
    connect(ui.tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sortChanged(int)));//нажатие на заголовок колонки приводит к пересортировке и записи в ини файл значения сортировки 
    model = new QSqlTableModel(this,tDB);
    initializeModel(model);
    ui.tableView->setModel(model);
    connect(ui.tableView->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(endEdit(QModelIndex,QModelIndex)));//нажатие на ENTER при редактировании таблицы

    ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setMinimumWidth(300);
    //connect(ui.tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(sortChanged(int)));//нажатие на заголовок таблицы для сортировки
}

GroupAEdit::~GroupAEdit()
{
	if (model) delete model;
	saveColorInfo();
}

void GroupAEdit::setParent(monitor* prt)
{
    parent=(monitor*)prt;
}

void GroupAEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) close();
}

void GroupAEdit::initializeModel(QSqlTableModel *temp_model)
{
	temp_model->setTable("avtonom_groups");
	temp_model->setEditStrategy(QSqlTableModel::OnFieldChange);
	temp_model->select();
	//temp_model->setHeaderData(0, Qt::Horizontal, QObject::tr("i"));
	temp_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя"));
	temp_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Описание"));
}
 
 void GroupAEdit::iShow(){
    ui.pbDel->setDisabled(true);//при рефреше кнопка включается если есть что удалять
    refresh("");

	//ui.tableView->resizeColumnsToContents();
    //ui.tableView->resizeRowsToContents();
	//ui.tableView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);//

    //////ui.tableView->setMinimumWidth(ui.tableView->frameWidth() * 3 +ui.tableView->visualRect(ui.tableView->model()->index( 0, ui.tableView->model()->columnCount() - 1 ) ).right());// + ui.tableView->verticalHeader()->width());
    //adjustSize();
    show();
}

void GroupAEdit::endEdit(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QVariant value = ui.tableView->model()->data(topLeft);
    QString last_value=(value.type()==QVariant::String)?value.toString():"";
    //qDebug()<<"last_value: "<<last_value<<",     ";
    refresh(last_value);
        //if (value.type()==QVariant::String)   refresh(value.toString());
        //else refresh("");
}

//void GroupAEdit::refresh(const QModelIndex &topLeft){
void GroupAEdit::refresh(QString last_edited_position){
    //qDebug()<<"last_edited_position: "<<last_edited_position<<"\n";
    //ui.tableView->reset();
    model->select();                    //refresh
    //model->removeColumn(0);
    //model->removeColumn(3);
    ui.tableView->setModel(model);      //refresh
    //QSqlTableModel *TModel = NULL;
    //QSqlDatabase tDB = QSqlDatabase::database();

    /*if (tDB.isOpen()){
        if (model){
            TModel = model;//старую удалить, новую влепить
        }
        model = new QSqlTableModel(this,tDB);
        model->setTable("avtonom_groups");*/

//  if (tDB.isOpen())
        //ui.tableView->reset();
        //if (model->select())
            //ui.tableView->setColumnWidth(0,0);//прячем колонку с индексом
            //ui.tableView->setColumnHidden(0,true);
            //ui.tableView->setColumnHidden(3,true);
            //ui.tableView->setEnabled(TRUE);
            //int xzxz=model->rowCount();
            if (model->rowCount()>0)
            {
                ui.pbDel->setDisabled(false);//если что-нибудь есть, удаляем
                //QModelIndex firstIndex=model->index(0,0);//первая строчка, второй столбик
                int cursor_row_index=0;
                for (int i=0;i<model->rowCount();i++)
                {
                    QSqlRecord rec = model->record(model->index(i,1).row());
                    if (rec.value(1).toString()==last_edited_position)
                    {
                        cursor_row_index=i;
                        break;
                    }
                }
                ui.tableView->setCurrentIndex(model->index(cursor_row_index,0));//если рефреш после редактирования - оставим курсор где был, иначе поставим на первую позицию
				//ui.tableView->scrollTo(model->index(cursor_row_index,0), QAbstractItemView::EnsureVisible);

                //ui.tableView->setCurrentIndex(topLeft);//если рефреш после редактирования - оставим курсор где был, иначе поставим на первую позицию
            }
            else ui.pbDel->setDisabled(true);//при рефреше кнопка включается если есть что удалять
            /*ui.tableView->setSortingEnabled(TRUE);
            if(parent->sort_a)
                ui.tableView->sortByColumn(1,Qt::AscendingOrder);
            else
                ui.tableView->sortByColumn(1,Qt::DescendingOrder);
*/

	
	ui.tableView->setColumnHidden(0,true);
	ui.tableView->setColumnHidden(3,true);

    ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui.tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    // old ui.tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
    // old ui.tableView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);

	ui.tableView->resizeColumnsToContents();
    ui.tableView->resizeRowsToContents();

	if (ui.tableView->columnWidth(1)<80) ui.tableView->setColumnWidth(1,80);
	if(parent->sort_a)
		ui.tableView->sortByColumn(1,Qt::AscendingOrder);
	else
		ui.tableView->sortByColumn(1,Qt::DescendingOrder);
    adjustSize();
}

void GroupAEdit::on_pbAdd_clicked()
{
    QSqlQuery *tq = new QSqlQuery();
    QString str="insert into avtonom_groups (name, human_descr) values ("+tr("'Новая группа', ")+tr("'Описание новой группы'")+");";
    /*tq->prepare("insert into avtonom_groups (name, human_descr) values (:name, :human_descr); ");
    tq->bindValue(":name",tr("Новая группа"));
    tq->bindValue(":human_descr",tr("Описание новой группы"));*/
    bool k = tq->exec(str);
	//int newId=tq->lastInsertId().toInt();
    delete tq;
	emit refreshG();
    refresh(tr("Новая группа"));
	ui.tableView->scrollToBottom();
    //ui.pbDel->setDisabled(true);
}

void GroupAEdit::on_pbDel_clicked()
{
    QString str;
    int count=0;
    QModelIndex curIndex = ui.tableView->currentIndex();
    int row = curIndex.row();
    QSqlRecord record = model->record(row);
    int UserNameIndex = record.indexOf("rec_id");
    int ls_UserName=record.value(UserNameIndex).toInt();
    QSqlQuery *tq = new QSqlQuery();
    str="select * from device where group_a="+QString::number(ls_UserName,10);
    tq->exec(str);
    if(tq->isActive())
    {
        tq->first();
        while(tq->next())
        {   count++;    }
    }
    if(count>0)
    {
        QMessageBox::critical(0, qApp->tr("Предупреждение системы"),
            tr("Для удаления группы необходимо удалить все принадлежащие ей приборы! "), QMessageBox::Cancel,
            QMessageBox::NoButton);
    }
    else
    {
        str="delete from avtonom_groups where rec_id="+QString::number(ls_UserName,10);
        tq->exec(str);
    }
    delete tq;
	emit refreshG();
    refresh("");
    //ui.pbDel->setDisabled(true);
}

void GroupAEdit::sortChanged(int logicalIndex)
{
    if(logicalIndex==1)
    {
        if(ui.tableView->horizontalHeader()->sortIndicatorOrder()==Qt::AscendingOrder)
        {
            parent->sort_a=true;
        }
        else
        {
            parent->sort_a=false;
        }
        parent->key_widg=true;
		emit refreshG();
	}
}

void GroupAEdit::saveColorInfo(){
    
    QFileInfo info("monitor.ini");

    if(info.isWritable())
    {
        
        BaseSettings *bs = BaseSettings::createInstance();
        
        if(parent->sort_a)
            bs->settings->setValue("sort/sort_a",QString::number(1,10));
        else
            bs->settings->setValue("sort/sort_a",QString::number(0,10));

        bs->freeInst();
    }
    else
    {
        QMessageBox::critical(this, tr("Предупреждение системы"),tr("Файл конфигурации защищен от записи!\n  Обратитесь в сервисную службу!")); 
    }
}

void GroupAEdit::on_pbExit_clicked()
{close();}

void GroupAEdit::closeEvent(QCloseEvent *event)
{event->accept();}//или удалять из родителя и обнулять указатель или скрывать не удаляя

