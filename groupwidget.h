#ifndef GROUPWIDGET_H
#define GROUPWIDGET_H

#include <QtWidgets/QWidget>
#include <QFile>
#include "ui_groupwidget.h"
#include "datatypes.h"
#include "findipw.h"
#include "monitor.h"

//class QListWidgetItem;
class FlowLayout;
class DeviceButton;
class QScrollArea;
class QTimer;
class monitor;
class AdditionalButtons;

class GroupWidget : public QWidget
{
    Q_OBJECT

public:
    GroupWidget(QWidget *parent = 0);
    ~GroupWidget();
    void AddDevice(DeviceButton *in_b);
    void DelDevice(DeviceButton *in_b);
    void setHideWgt();
    void setButED();

    bool key_group;
    //bool key_circlecommand;
    void setParent(monitor*);
    void showArea2();
    void hideArea2();
    
private:
    Ui::GroupWidgetClass ui;
    FlowLayout *FL;
    FlowLayout *FL2;
    QScrollArea *scrollArea;
    QScrollArea *scrollArea2;
    MonitorShowMode mode;
    int gID;
    int buttons_config;	//конфигурация кнопок отправки команд голосом/датой
    //QTimer *autoTimer;
    //QTimer *timeToTimer;
    monitor* parent;
    //QString name;
    //int tabIndex;
    //QString tabText;
    void reAskDisp(QString txtCommand, int intCommand);
    void generateDeltaCommands();
    bool isAutomationActive;
    int h,m,s;
    int _h,_m;
    QList<DeviceButton *> list_index;

    QFile *debug;
    QString pathlog;
    void logparser(QString str);

private slots:
    //void on_pbAutomation_clicked();
    void on_pbClearAhtung_clicked();
    //void on_pbDelQueryCommand_clicked();
    void on_pbGroupDelta_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_KH_clicked();
    void on_pushButton_KB_clicked();
    void on_pushButton_clicked();
    //void startAutoTimer(int in_val);
    //void stopAutoTimer();
    //void onAutoTimer();
    //void onTimeToTimer();
    //void onItemDblClick(QListWidgetItem *item);
public slots:
    void updateFromBD();
    void setMonitorMode(MonitorShowMode in_mode);
    void setGID(int in_gID);
    void pic_State(int,DeviceButton *);
signals:
    //void toServer();
    //void toAllServer();
    void devCountChanged(int, QString);
    void clearAhtung(MonitorShowMode, int);
    void putCommand(int device, QString command, int command_type);
};

#endif // GROUPWIDGET_H
