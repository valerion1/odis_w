#ifndef IPCONFIGURATOR_H
#define IPCONFIGURATOR_H

#include <QtWidgets/QWidget>
#include <QTimer>
#include "ui_ipconfigurator.h"
#include "ipwindow.h"
#include "ipobject.h"
#include "basesettings.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class IPWindow;
//class BaseSettings;

class IPConfigurator : public QWidget
{
	Q_OBJECT

public:
	//BaseSettings *mainBs;
	IPConfigurator(QWidget *parent = 0);
	~IPConfigurator();
	void IShow(int bdIndex);

	IPWindow* parent;
	void setParent(IPWindow *parent);
	bool key_config;
	bool key_button;
	bool key_login;
		
private:
	//QCheckBox *cb_door;
	//QCheckBox *cb_datamode;
	void keyPressEvent(QKeyEvent *event);
	Ui::IPConfiguratorClass ui;
	QCheckBox *checkArray[9];
	QComboBox *comboArray[9];
	QLineEdit *lineEditArray[9];
	int curIndex;
	bool b;
	bool c;
	bool k1;
	bool k2;
	bool cp1;
	bool cp2;
	bool cp3;
	bool cp4;
	bool cp5;
	bool cp6;
	bool cp7;
	bool cp8;
	bool cp9;
	bool old_b;
	bool old_c;
	bool old_k1;
	bool old_k2;
	bool old_cp1;
	bool old_cp2;
	bool old_cp3;
	bool old_cp4;
	bool old_cp5;
	bool old_cp6;
	bool old_cp7;
	bool old_cp8;
	bool old_cp9;
	int indexes[9];
	bool tCp[9];
	bool door;
	bool old_door;
	bool data_mode;
	bool old_datamode;
	//bool coun ter_present;
	int counter_number;
	int counter_type;
	int counter_pass;

	//QTimer* tm_state;
	//int state_inter;

private slots:
	
	void on_cb_9_stateChanged(int);
	void on_cb_8_stateChanged(int);
	void on_cb_7_stateChanged(int);
	void on_cb_6_stateChanged(int);
	void on_cb_5_stateChanged(int);
	void on_cb_4_stateChanged(int);
	void on_cb_3_stateChanged(int);
	void on_cb_2_stateChanged(int);
	void on_cb_1_stateChanged(int);
	void on_cb_K2_stateChanged(int);
	void on_cb_K1_stateChanged(int);
	void on_cb_C_stateChanged(int);
	void on_cb_B_stateChanged(int);
	void on_pbCancel_clicked();
	void on_pbOK_clicked();
	void on_pbDefault_clicked();

	//void tm_stateTimer();
signals:
	void refreshG();
	//void ipconf_putCommand(int device, int command, int command_type);
	void ipconf_signalAllToServer();
//	void putCommand(int device, int command, int command_type);
    void putCommand(int device, QString command, int command_type);

};

#endif // IPCONFIGURATOR_H
