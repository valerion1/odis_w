#include "servicetables.h"
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
#include "basesettings.h"
#include "servicetablerecord.h"
#include <QKeyEvent>

ServiceTables::ServiceTables(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	model = NULL;
	buttonList = new QList<QPushButton *>;
	buttonList->append(ui.pb_send_commands);
	buttonList->append(ui.pb_send_state);
	//buttonList->append(ui.pb_counter_types);
	//buttonList->append(ui.pb_time_types);
	buttonList->append(ui.pb_gprs_types);
	buttonList->append(ui.pb_check_point_type);
	buttonList->append(ui.pb_phase_state);
	buttonList->append(ui.pb_k_state);
	buttonList->append(ui.pb_f_state);
	buttonList->append(ui.pb_feed_back_state);
	buttonList->append(ui.pb_door_state);
	gui_ServiceTableRecord = new ServiceTableRecord(NULL);
	connect(gui_ServiceTableRecord,SIGNAL(RetVal(QString,int)),this,SLOT(getNewRecord(QString,int)));

	
	
}


void ServiceTables::keyPressEvent(QKeyEvent *event){
	if (event->key() == Qt::Key_Escape){
		hide();
	}
}

void ServiceTables::getNewRecord(QString recText,int intVal){
	QSqlQuery *tq = new QSqlQuery();
	QString str="insert into "+tableName+" (raw_val, human_descr) values ("+QString::number(intVal,10)+", '"+recText+"');";
	/*tq->prepare("insert into "+tableName+" (raw_val, human_descr) values (:raw_val, :human_descr); ");
	tq->bindValue(":raw_val",intVal);
	tq->bindValue(":human_descr",recText);*/
	tq->exec(str);
	refreshTable();
	delete tq;
}

ServiceTables::~ServiceTables()
{
	delete buttonList;
	delete gui_ServiceTableRecord;
	if (model) delete model;
}


void ServiceTables::refreshTablesButtons(QPushButton *currButton){
	int len = buttonList->count();
	for (int i=0;i<len;i++){
		if (buttonList->at(i) != currButton){
			//not our button ))
			buttonList->at(i)->setChecked(FALSE);
		}
	}
	if (!currButton->isChecked()){
		currButton->setChecked(TRUE);
	}
	refreshTable();
}

void ServiceTables::refreshTable(){
	if (model) delete model;
	QSqlDatabase tDB = QSqlDatabase::database();
	if (tDB.isOpen()){
		model = new QSqlTableModel(this,tDB);
		model->setTable(tableName);
		if (model->select()){
			//model->removeColumn(0); индекс не удаляем - а то как потом искать будем
			model->setHeaderData(0, Qt::Horizontal, QObject::tr("Индекс"));
			model->setHeaderData(1, Qt::Horizontal, QObject::tr("Значение"));
			model->setHeaderData(2, Qt::Horizontal, QObject::tr("Описание"));
			model->setEditStrategy(QSqlTableModel::OnFieldChange);
			ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
			ui.tableView->setModel(model);
			ui.tableView->resizeColumnsToContents();
			ui.tableView->resizeRowsToContents();
			//ui.tableView->setColumnWidth(0,0);//прячем колонку с индексом
			ui.tableView->setEnabled(TRUE);
			ui.tableView->show();
		}else{
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
	}
}

void ServiceTables::iShow(){
	tableName = "send_commands";
	refreshTablesButtons(ui.pb_send_commands);
	show();
}

void ServiceTables::on_pbQuit_clicked()
{
	hide();
}

void ServiceTables::on_pb_send_state_clicked()
{
	tableName = "send_state";
	refreshTablesButtons(ui.pb_send_state);
}
/*
void ServiceTables::on_pb_counter_types_clicked()
{
	tableName = "counter_types";
	refreshTablesButtons(ui.pb_counter_types);
}

void ServiceTables::on_pb_time_types_clicked()
{
	tableName = "time_types";
	refreshTablesButtons(ui.pb_time_types);
}
*/
void ServiceTables::on_pb_gprs_types_clicked()
{
	tableName = "gprs_types";
	refreshTablesButtons(ui.pb_gprs_types);
}

void ServiceTables::on_pb_check_point_type_clicked()
{
	tableName = "check_point_type";
	refreshTablesButtons(ui.pb_check_point_type);
}

void ServiceTables::on_pb_phase_state_clicked()
{
	tableName = "phase_state";
	refreshTablesButtons(ui.pb_phase_state);
}

void ServiceTables::on_pb_k_state_clicked()
{
	tableName = "k_state";
	refreshTablesButtons(ui.pb_k_state);
}

void ServiceTables::on_pb_f_state_clicked()
{
	tableName = "f_state";
	refreshTablesButtons(ui.pb_f_state);
}

void ServiceTables::on_pb_feed_back_state_clicked()
{
	tableName = "feed_back_state";
	refreshTablesButtons(ui.pb_feed_back_state);
}

void ServiceTables::on_pb_door_state_clicked()
{
	tableName = "door_state";
	refreshTablesButtons(ui.pb_door_state);
}

void ServiceTables::on_pbAdd_clicked()
{
	gui_ServiceTableRecord->iShow(tableName);
}

void ServiceTables::on_pbDel_clicked()
{
	QModelIndex curRow = ui.tableView->currentIndex();
	if (curRow.isValid()){
		model->removeRow(curRow.row());
		model->submitAll();
	}
}

void ServiceTables::on_pb_send_commands_clicked()
{
	tableName = "send_commands";
	refreshTablesButtons(ui.pb_send_commands);
}
