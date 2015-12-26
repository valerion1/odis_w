#ifndef SERVICETABLES_H
#define SERVICETABLES_H

#include <QtWidgets/QWidget>
#include "ui_servicetables.h"

class QSqlTableModel;
class ServiceTableRecord;

class ServiceTables : public QWidget
{
	Q_OBJECT

public:
	ServiceTables(QWidget *parent = 0);
	~ServiceTables();

private:
	Ui::ServiceTablesClass ui;
	QList<QPushButton *> *buttonList;
	QString tableName;
	QAction *newAct;
	QAction *changeAct;
	QAction *delAct;
	QSqlTableModel *model;
	ServiceTableRecord *gui_ServiceTableRecord;
	void refreshTablesButtons(QPushButton *currButton);
	void keyPressEvent(QKeyEvent *event);
public slots:
	void iShow();
	void getNewRecord(QString recText, int intVal);

private slots:
	void on_pb_send_commands_clicked();
	void on_pbDel_clicked();
	void on_pbAdd_clicked();
	void on_pb_door_state_clicked();
	void on_pb_feed_back_state_clicked();
	void on_pb_f_state_clicked();
	void on_pb_k_state_clicked();
	void on_pb_phase_state_clicked();
	void on_pb_check_point_type_clicked();
	void on_pb_gprs_types_clicked();
	//void on_pb_time_types_clicked();
	//void on_pb_counter_types_clicked();
	void on_pb_send_state_clicked();
	void on_pbQuit_clicked();
	void refreshTable();
};

#endif // SERVICETABLES_H
