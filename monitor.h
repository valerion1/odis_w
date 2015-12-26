#ifndef MONITOR_H
#define MONITOR_H

#include <QtWidgets/QMainWindow>
// old #include <QtGui/QMainWindow>
#include "ui_monitor.h"
#include "datatypes.h"
#include <QAbstractSocket>
#include <QSqlDatabase>
#include <QtWidgets/QLayout>
#include <QFile>

#include <QProcess>
#include "findipw.h"
#include "fndipw.h"
#include "options.h"

class ServiceTables;
class GroupGEdit;
class GroupAEdit;
class TerminalsEdit;
class IPWindow;
class QTcpSocket;
class QAbstractSocket;
class QTimer;
class QLabel;
class PrBar;
class ColorSelect;
class LoginWindow;
class AboutWindow;
class fndIPW; 
class options; 

class BaseSettings;
class IPWindow;
class AdditionalButtons;
class IPConfigurator;


enum sort_mode {
                name_num=1,
                name_num_r,
                position,
                name_str,
                name_str_r,
                phone,
                phone_r
                };


typedef struct OPtion
{
    bool solution;
    bool group;
    bool circlecommand;
    bool count;
    bool groupcount;
    bool anycommand;
    bool sort;
    bool asc;
    bool config;
}OPtion;

class monitor : public QMainWindow
{
    Q_OBJECT

public:
    // old monitor(QWidget *parent = 0, Qt::WFlags flags = 0);
    monitor(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~monitor();
    bool makeConnection();
    static int getTimeDifference();
    DevList deviceList,tmp_dev;
    fndIPW* fnddlg;
    options* optdlg;
    OPtion option;
    bool key_login,key_option;
    int mode_sort;
    MonitorShowMode currMode;
    BaseSettings *mainBs;

    IPWindow *informationWindow;
    //IPWindow *ipW;
    AdditionalButtons *moreButtons;//окно "дополнительно"
    AdditionalButtons *moreButtons_group;//окно "дополнительно" для групповой команды
    IPConfigurator *ipConfiguratorWindow;//маска/пароль счётчика

    QStringList counter_list_default;//список счётчиков по умолчанию
    bool sort_g,sort_a;
    int cur_position;
    bool key_widg;
    QString cur_page;
    QTcpSocket *soc;

    //void SortDevices(list_to_sort);
    Event_Table_H Event_Table,Event_Table1;
    Avtonom avto;
    void SortDevices(DevList &,MonitorShowMode);
    void LoadAvtonom();//IPObject*

private:
    Ui::monitorClass ui;

    /////////////////////////////////////////////
    QFile *file;
    QFile *debug;
    QString pathlog;
    void logparser(QString str);
    /////////////////////////////////////////////
    QTimer *RealTimeTimer;
    QTimer *DataBasePingTimer;
    QTimer timer_ver;
    //
    ServiceTables *gui_ServiceTables;
    GroupGEdit *gui_GroupGEdit;
    GroupAEdit *gui_GroupAEdit;
    LoginWindow *gui_LoginWindow;
    TerminalsEdit *gui_TerminalsEdit;
    QList<QPushButton *> *buttonList;
    void refreshModeButtons(QPushButton *currButton);
    ColorSelect *gui_colorSelectionWindow;
    AboutWindow *gui_AboutWindow;
    //QTcpSocket *soc;
    QLabel *TimeLabel;
    QLabel *sendCountLabel;
    QLabel *servLabel;
    QAction *action_12;
    QAction *action_13;
    //QAction *action_20;
    QPushButton *pbCount;
    QLayout *horizontalLayout;
    QProcess* process;
    QProcess* help_process;
    QProcess* process_ext;
    
    //

    QList<GrList *> groupGList;
    QList<GrList *> groupAList;
    QList<GrList *> groupSList;
    QList<GrList *> groupCList;
    QList<GrList *> groupAllList;
    
    QList <QStringList*> strList;
    QList<GrList *> tempList;

    QSqlDatabase workDB;

    //void sortGlist();херовая сортировка, действовала и на приборы и на вкладки
    //void sortAlist();
    //void sortSlist();
    //void sortClist();
    //void sortAllList();

    QStringList dump;
    QStringList rec;
    bool startValidater();
    void replacePhone();
    void getDB();
    void PhoneUpdate();
    
    //
    PrBar *progress;
    void connectToServer();
    void parser(QString incoming);

    void buildDeviceList();
    void buildGroupsList();
    void deleteGroupsList();
    void clearDeviceList();

    void GetCPUID();
    bool Security();
    int CPUInfo[4];
    
    QString getNametoInt(QString);
        
    QString readBuffer;
    int BD_id;
signals:
    void needExit();
    void currentChanged(int index);  

    
public:
    void UpdateCount();
    void setAccessUpdate();
    void iShow();
    void SignaltoMap(int);

private slots:
    void on_cb_SortingMode_currentIndexChanged(int);
    void on_pb_alldevice_clicked();
    void on_pushButton_tpfind_clicked();
    void databasePing();
    void on_pbState_clicked();
    void onClearQueryClicked();
    void onOptionsClicked();
    void on_pbGA_clicked();
    void on_pbCount_clicked();
    void on_pbGG_clicked();
    void getMap(int dev_id);
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socErr);
    void onDataReady();
    void setNewTime();
    void NeedQuit();
    void onCountersClicked();
    void ontimer_ver();
    void show_help();
    void onChanged(int);//переключать вкладки
    //void check_tab_order(QMouseEvent *event);//менять вкладки местами
    void create_gui_GroupGEdit();
    void create_gui_GroupAEdit();

public slots:
    void updateAfterConfig();
    void clearAhtungState(MonitorShowMode in_mode, int in_grID);
    void signalToServer();//после постановки команды в базу
    void getCommand(int dev_id, QString new_command,int cmd_type);
    void onDevCountChanged(int, QString);
    void updateMonitor();
    void setAccessLevel();
    void UpdateDB();
    void get_Device_Click(IPObject * master);
    //void onTabChange(int tabIndex);
};

bool SortDevicesNameStr(const IPObject *, const IPObject *);
bool RSortDevicesNameStr(const IPObject *, const IPObject *);
bool SortDevicesNameNum(const IPObject *, const IPObject *);
bool RSortDevicesNameNum(const IPObject *, const IPObject *);
bool SortDevicesPosition(const IPObject *, const IPObject *);
bool SortDevicesPhone(const IPObject *, const IPObject *);
bool RSortDevicesPhone(const IPObject *, const IPObject *);
#endif // MONITOR_H
