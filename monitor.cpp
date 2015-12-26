#include "monitor.h"
#include "basesettings.h"
#include "servicetables.h"
#include "groupgedit.h"
#include "groupaedit.h"
#include "terminalsedit.h"
#include "datatypes.h"
#include "groupwidget.h"
#include "ipobject.h"
#include "ipwindow.h"
#include "ipconfigurator.h"
#include "prbar.h"
#include "additionalbuttons.h"
#include "colorselect.h"
#include "loginwindow.h"
#include "aboutwindow.h"
#include <stdlib.h>
#include <windows.h>
#include <intrin.h>
#include <time.h>
//#include "senddatadlg.h"
#include <windows.h>

#include <QtWidgets/QInputDialog>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QRegExp>
#include <QSound>
#include <QTimer>
#include <QtWidgets/QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
#include <QDateTime>


void monitor::create_gui_GroupGEdit()
{
    if (!gui_GroupGEdit)
    {
        gui_GroupGEdit = new GroupGEdit(NULL);
        gui_GroupGEdit->setParent(this);
        //gui_GroupGEdit->setAttribute(Qt::WA_DeleteOnClose,true);
        connect(gui_GroupGEdit,SIGNAL(refreshG()),this,SLOT(updateAfterConfig()));
    }
    gui_GroupGEdit->iShow();
}

void monitor::create_gui_GroupAEdit()
{
    if (!gui_GroupAEdit)
    {
        gui_GroupAEdit = new GroupAEdit(NULL);//база данных должна быть открыта
        gui_GroupAEdit->setParent(this);
        //gui_GroupGEdit->setAttribute(Qt::WA_DeleteOnClose,true);
        connect(gui_GroupAEdit,SIGNAL(refreshG()),this,SLOT(updateAfterConfig()));
    }
    gui_GroupAEdit->iShow();
}

monitor::monitor(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    //QStringList counter_list_default;//список счётчиков по умолчанию
    QStringList counter_list;
    counter_list_default<<tr("счётчик отсутствует")
                        <<tr("Энергомера v4")
                        <<tr("Энергия 9")
                        <<tr("ЕМС 134")
                        <<tr("ЕМС ЭЛВИН")
                        <<tr("АРГО 2314")
                        <<tr("НИК 2303")
                        <<tr("Энергомера v6")
                        <<tr("Энергомера CE30x (9600)")
                        <<tr("Энергомера CE30x (300/9600)");
    ui.setupUi(this);
    sort_g=true;
    sort_a=true;
    ui.menu->setTitle(tr("Пуск"));
    //ui.pbGA->setText(tr("По ПАР"));

    process_ext=NULL;

    //this->setWindowTitle(tr("Программа \"Monitor\" - интерфейсная часть системы управления освещением СПРУТ (Debug_version)"));
    this->setWindowTitle(tr("OdisW Monitor - интерфейсная часть системы управления освещением СПРУТ"));
    mainBs = BaseSettings::createInstance();
    //int xz=this->mainBs->accessLevel;

    int counter_array_size=mainBs->settings->beginReadArray("counters");
    if (counter_array_size<=0)//в конфиге пусто, задаём массив по умолчанию
    {
        mainBs->settings->endArray();
        mainBs->settings->beginWriteArray("counters");
        for (int i = 0; i < counter_list_default.size(); i++)
        {
            mainBs->settings->setArrayIndex(i);
            //mainBs->settings->setValue("cnt"+QString::number(i)+"/name",tr(counter_list_default.at(i).toLocal8Bit()));
            mainBs->settings->setValue("name",tr(counter_list_default.at(i).toLocal8Bit()));
        }

        mainBs->settings->endArray();
        mainBs->settings->sync();//скинули в ини файл
    }
    else//нормально прочитали массив счётчиков из ини файла
    {
        mainBs->settings->endArray();
    }
    //counter_list=mainBs->settings->childGroups();
    /*if (counter_list.size()<=0)
    {
        for (int i=0;i<counter_list_default.size();i++)
        {
            mainBs->settings->setValue("cnt"+QString::number(i)+"/name",tr(counter_list_default.at(i).toLocal8Bit()));
            //mainBs->settings->setValue("cnt"+QString::number(i)+"/params",0);
        }
    }
    mainBs->settings->endGroup();*/
    
    soc = new QTcpSocket(NULL);
    connectToServer();
    connect(soc,SIGNAL(connected()),this,SLOT(onSocketConnected()));
    connect(soc,SIGNAL(disconnected()),this,SLOT(onSocketDisconnected()));
    connect(soc,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(soc,SIGNAL(readyRead()),this,SLOT(onDataReady()));
    
    connect(&timer_ver,SIGNAL(timeout()),this,SLOT(ontimer_ver()));
    timer_ver.setInterval(5000);
    //
    this->setWindowState(this->windowState() ^ Qt::WindowMaximized);//Qt::WindowFullScreen);
    //
    //по идее так правильнее реализовывать менюшку
    gui_GroupGEdit=0;
    gui_GroupAEdit=0;
    connect(ui.action_4,SIGNAL(triggered()),this,SLOT(create_gui_GroupGEdit()));//редактор групп по районам
    connect(ui.action_5,SIGNAL(triggered()),this,SLOT(create_gui_GroupAEdit()));//редактор групп по автоному (пользовательских)

    connect(ui.action,SIGNAL(triggered()),this,SLOT(NeedQuit()));
    gui_ServiceTables = new ServiceTables(NULL);
    connect(ui.action_2,SIGNAL(triggered()),gui_ServiceTables,SLOT(iShow()));
    gui_TerminalsEdit = new TerminalsEdit(NULL,&deviceList);
    gui_TerminalsEdit ->setParent(this);
    connect(ui.action_3,SIGNAL(triggered()),gui_TerminalsEdit,SLOT(iShow()));
    connect(gui_TerminalsEdit,SIGNAL(UpdatedDB()),this,SLOT(UpdateDB()));

    gui_colorSelectionWindow = new ColorSelect(NULL);
    gui_colorSelectionWindow->setpParent(this);
    connect(ui.action_6,SIGNAL(triggered()),gui_colorSelectionWindow,SLOT(iShow()));//#########################
    connect(gui_colorSelectionWindow,SIGNAL(refreshG()),this,SLOT(updateAfterConfig()));

    connect(gui_TerminalsEdit,SIGNAL(refreshG()),this,SLOT(updateAfterConfig()));
    //
    connect(ui.action_7,SIGNAL(triggered()),this,SLOT(onClearQueryClicked()));
    connect(gui_TerminalsEdit,SIGNAL(refreshG()),this,SLOT(updateMonitor()));
    
    buttonList = new QList<QPushButton *>;
    buttonList->append(ui.pbGA);
    buttonList->append(ui.pbGG);
    buttonList->append(ui.pbState);
    buttonList->append(ui.pbCount);
    buttonList->append(ui.pb_alldevice);
    //??
    ui.tabWidget->removeTab(0);
    //connect(ui.tabWidget,SIGNAL(currentChanged(int)),this,SLOT(onTabChange(int)));
    connect(ui.tabWidget,SIGNAL(currentChanged(int)),this,SLOT(onChanged(int)));
    //окно с мнемосхемой терминала и историей
    informationWindow = new IPWindow();
    informationWindow->setParent(this);

    connect(informationWindow,SIGNAL(signalToServer()),this,SLOT(signalToServer()));//
    connect(informationWindow,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));//сигнал/слот передачи команды в базу
    connect(informationWindow,SIGNAL(putMap(int)),this,SLOT(getMap(int)));//
    //connect(informationWindow,SIGNAL())
    //connect(informationWindow,SIGNAL(signalAllToServer()),this,SLOT(signalAllToServer()));
    //connect(informationWindow,SIGNAL(putCommand(int, int, int)),this,SLOT(getCommand(int, int, int)));
    //
    //окно "Дополнительно" для IPWindow
    moreButtons = new AdditionalButtons();
    moreButtons->setParent(informationWindow);
    connect(moreButtons,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));//сигнал/слот передачи команды в базу
    //connect(mainBs->moreButtons,SIGNAL(emitCmd(int, int, int)),this,SLOT(getCommand(int, int, int)));
    //
    ipConfiguratorWindow = new IPConfigurator();
    ipConfiguratorWindow->setParent(informationWindow);
    connect(ipConfiguratorWindow,SIGNAL(refreshG()),informationWindow,SLOT(refreshFace()));
    connect(ipConfiguratorWindow,SIGNAL(putCommand(int,QString,int)),this,SLOT(getCommand(int,QString,int)));//сигнал/слот передачи команды в базу
    //connect(mainBs->ipConfiguratorWindow,SIGNAL(ipconf_signalAllToServer()),this,SLOT(signalAllToServer()));

    //mainBs->moreButtons = new AdditionalButtons(NULL);
    //connect(mainBs->moreButtons,SIGNAL(emitCmd(int, int, int)),this,SLOT(getCommand(int, int, int)));
    moreButtons_group = new AdditionalButtons();
    moreButtons_group->setParent2(this);
    //this->deviceList
    connect(moreButtons_group,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));//сигнал/слот передачи команды в базу

    //

    gui_LoginWindow = new LoginWindow(this);
    connect(gui_LoginWindow,SIGNAL(changeRole()),this,SLOT(setAccessLevel()));
    connect(ui.action_10,SIGNAL(triggered()),gui_LoginWindow,SLOT(show()));

    /*action_20=NULL;
    action_20 = new QAction(tr("Содержание справки"),this);
    ui.menu_3->addAction(action_20);*/
    connect(ui.action_20,SIGNAL(triggered()),this,SLOT(show_help()));
    //servLabel = new QLabel(NULL);
    //servLabel->setText("");
    QFont f;// = servLabel->font();
    //f.setBold(true);
    //f.setPixelSize(20);
    //servLabel->setPalette(QPalette(QColor(Qt::red)));
    //servLabel->setFont(f);
    //this->statusBar()->addPermanentWidget(servLabel);
    //
    //
    sendCountLabel = new QLabel(NULL);
    f = sendCountLabel->font();
    f.setBold(true);
    f.setPixelSize(20);
    sendCountLabel->setFont(f);
    this->statusBar()->addPermanentWidget(sendCountLabel);
    //
    TimeLabel = new QLabel(NULL);
    f = TimeLabel->font();
    f.setBold(true);
    f.setPixelSize(20);
    TimeLabel->setFont(f);
    this->statusBar()->addPermanentWidget(TimeLabel);
    //
    
    RealTimeTimer = new QTimer(NULL);
    RealTimeTimer->setInterval(1000);
    connect(RealTimeTimer,SIGNAL(timeout()),this,SLOT(setNewTime()));
    RealTimeTimer->start();

    
    DataBasePingTimer = new QTimer(NULL);
    DataBasePingTimer->setInterval(900000);
    connect(DataBasePingTimer,SIGNAL(timeout()),this,SLOT(databasePing()));
    DataBasePingTimer->start();

    gui_AboutWindow = new AboutWindow(NULL);
    connect(ui.action_11,SIGNAL(triggered()),gui_AboutWindow,SLOT(show()));

    action_12=NULL;
    action_12 = new QAction(tr("Настройки"),this);
    ui.menu_2->addAction(action_12);
    connect(action_12,SIGNAL(triggered()),this,SLOT(onOptionsClicked()));

    action_13=NULL;
    action_13 = new QAction(tr("Анализ счетчиков"),this);
    ui.menu_2->addAction(action_13);
    connect(action_13,SIGNAL(triggered()),this,SLOT(onCountersClicked()));

    ui.pbCount->setText(tr("По счётчикам"));
    ui.pbCount->setCheckable(true);
    ui.pb_alldevice->setText(tr("Все приборы"));
    ui.pb_alldevice->setCheckable(true);
    connect(ui.pb_alldevice,SIGNAL(clicked()),this,SLOT(on_pb_alldevice_clicked()));
    /*pbCount=new QPushButton(tr("По счётчикам"),ui.frame);
    pbCount->setCheckable(true);
    horizontalLayout=ui.frame->layout();
    horizontalLayout->addWidget(pbCount);*/

    fnddlg=NULL;
    fnddlg=new fndIPW();
    fnddlg->setpParent(this);

    optdlg=NULL;
    optdlg=new options();
    optdlg->setpParent(this);
    optdlg->getOptions();
    setAccessUpdate();
    UpdateCount();

    key_login=false;
    key_option=false;
    mode_sort=0;
    cur_position=0;
    key_widg=false;

    for(int i=0;i<4;i++) CPUInfo[i]=-1;

    help_process=NULL;
    help_process= new QProcess();

    debug = new QFile();
    //BaseSettings *bs = BaseSettings::createInstance();
    pathlog = mainBs->settings->value("DirLogFile/path","./").toString();
    pathlog.replace(QString("\\"), QString("/"));

    srand(time(NULL));
    BD_id=rand()%100+1;

    bool key=false;

    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime utcTime = currentTime.toUTC();
    utcTime.setTimeSpec(Qt::LocalTime);
    int normalDiff = mainBs->settings->value("System/normal_UTC_diff",3).toInt();;
    uint difTime = currentTime.toTime_t() - utcTime.toTime_t();
    
    if (difTime == normalDiff*60*60)
        key=false;
    else
        key=true;


    /////////////////////////////////////////////

    //normalDiff =mainBs->settings->value("System/normal_UTC_diff",3).toInt();
    //difTime = currentTime.toTime_t() - utcTime.toTime_t();
    //int delta_time=difTime-normalDiff*60*60;
    //delta_time=delta_time/3600;

    //currentTime.setTime_t(currentTime.toTime_t() - delta_time*60*60);
    //QString str;
    //str.clear();
    //str.append(currentTime.toString("hh:mm:ss"));

    ////////////////////////////////////////////

    //mainBs->freeInst();
}

void monitor::logparser(QString str)
{
    QString temp;
    debug->setFileName(pathlog+"monitor_"+QDateTime::currentDateTime().toString("yyyy.MM.dd")+".log");

    temp=temp+QDateTime::currentDateTime().toString("hh:mm:ss        ")+str;
    
    if (debug->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(debug);
        out.setCodec("UTF-8");
        //out << crypt(str) << "\n";
        out << temp << "\n";
        out.flush();
        debug->flush();
        debug->close();
    }
}

void monitor::databasePing(){
    QSqlQuery *tq = new QSqlQuery("select rec_id from device order by position");
    if (tq->isActive()){
        signalToServer();
    }else{

        QSqlError err = workDB.lastError();
        QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
                err.text(), QMessageBox::Cancel,
                QMessageBox::NoButton);

        QMessageBox::critical(0, qApp->tr("Горсвет"),
            qApp->tr("Ошибка соединения с базой"), QMessageBox::Cancel,
            QMessageBox::NoButton);
    }
    delete tq;
    
}


monitor::~monitor()
{
    
    //deleteGroupsG();
    //deleteGroupsA();
    //deleteGroupsS();
    //deleteDeviceList();
    RealTimeTimer->stop();
    DataBasePingTimer->stop();
    delete DataBasePingTimer;
    delete RealTimeTimer;
    delete TimeLabel;
    delete sendCountLabel;
    delete gui_AboutWindow;
    //delete servLabel;
    delete buttonList;
    delete gui_ServiceTables;
    if (gui_GroupGEdit) delete gui_GroupGEdit;
    if (gui_GroupAEdit) delete gui_GroupAEdit;
    if (gui_TerminalsEdit) delete gui_TerminalsEdit;
    delete gui_colorSelectionWindow;
    delete gui_LoginWindow;
    if (QSqlDatabase::database().isOpen()){
        QSqlDatabase::database().commit();
        QSqlDatabase::database().close();
    }
    if (informationWindow->isVisible()){
        informationWindow->hide();
    }
    delete informationWindow;
    delete soc;
    mainBs->freeInst();
    deleteGroupsList();
    clearDeviceList();

    /*if(finddlg)
        delete finddlg;*/
    if(fnddlg)
        delete fnddlg;
    if(optdlg)
        delete optdlg;
    if(action_12)
        delete action_12;
    if(help_process)
        delete help_process;
    if (debug) delete debug;
    if(process_ext) delete process_ext;
}

void monitor::NeedQuit(){
    emit needExit();
}

void monitor::setNewTime(){
    TimeLabel->setText(QDateTime::currentDateTime().toString("dd-MM-yyyy (dddd) hh:mm:ss"));
    QSqlQuery *tq = new QSqlQuery("select count(rec_id) from send where (command_state=12) or (command_state=14) or (command_state=15) or (command_state=16)");
    if (tq->isActive()){
        if (tq->next()){
            sendCountLabel->setText(tr("Команд в очереди: ")+tq->value(0).toString()+"               ");
            }
    }
    delete tq;
    
}

//void monitor::onTabChange(int tabIndex)
//{
//	updateMonitor();
//}


void monitor::onClearQueryClicked(){
    QSqlQuery *sendQuery = new QSqlQuery("update send set command_state=10 "
        "where command_state!=0 and command_state!=9 and command_state!=11 and command_state!=13");
    signalToServer();
    delete sendQuery;
}

void monitor::iShow()
{	
    BaseSettings *bs = BaseSettings::createInstance();
    int sorting_mode=0;
    ui.menu_3->hide();
    PhoneUpdate();
    currMode = MODE_GG;
    buildDeviceList();
    buildGroupsList();
    refreshModeButtons(ui.pbGG);
    cur_page.clear();
    ui.cb_SortingMode->setCurrentIndex(bs->settings->value("sort/geoarea",sorting_mode).toInt());
    cur_page=ui.tabWidget->tabText(ui.tabWidget->currentIndex());

    show();
    //##################################DEBUG###################################
    setAccessLevel();
    //##################################DEBUG###################################
}

void monitor::onChanged(int index)
{
    cur_page.clear();
    cur_page=ui.tabWidget->tabText(index);
}

/*void monitor::check_tab_order(QMouseEvent *event)
{
    int i=ui.tabWidget->currentIndex();
    int i2=ui.tabWidget->count();
    return;
}*/

void monitor::updateAfterConfig(){
    BaseSettings *bs = BaseSettings::createInstance();
    int sorting_mode=0;
    deleteGroupsList();
    buildGroupsList();
    if(!key_widg)
        cur_position=ui.tabWidget->currentIndex();
    else
    {
        cur_position=0;
        key_widg=false;
    }

    UpdateCount();

    switch(currMode){
        case MODE_GG:
            sorting_mode=bs->settings->value("sort/geoarea",sorting_mode).toInt();
            refreshModeButtons(ui.pbGG);
            break;
        case MODE_GA:
            sorting_mode=bs->settings->value("sort/usergroups",sorting_mode).toInt();
            refreshModeButtons(ui.pbGA);
            break;
        case MODE_STATE:
            sorting_mode=bs->settings->value("sort/devstate",sorting_mode).toInt();
            refreshModeButtons(ui.pbState);
            break;
        case MODE_COUNT:
            sorting_mode=bs->settings->value("sort/countergroups",sorting_mode).toInt();
            refreshModeButtons(ui.pbCount);
            break;
        case MODE_ALL:
            sorting_mode=bs->settings->value("sort/all",sorting_mode).toInt();
            refreshModeButtons(ui.pb_alldevice);
            break;
        /*default:sorting_mode=bs->settings->value("sort/all",sorting_mode).toInt();
                if (!sorting_mode) 
                {
                    sorting_mode=1;
                    bs->settings->setValue("sort/all",sorting_mode);
                }
                break;*/

    } 
    ui.cb_SortingMode->setCurrentIndex(sorting_mode);//устанавливаем индекс для каждого вида выборки
}

void monitor::refreshModeButtons(QPushButton *currButton){
    int len = buttonList->count();
    for (int i=0;i<len;i++){
        if (buttonList->at(i) != currButton){
            //not our button ))
            buttonList->at(i)->setChecked(FALSE);
        }
    }
    
    ui.tabWidget->hide();
    ui.tabWidget->clear();
    ui.frame->show();
    ui.verticalLayout->update();
    int gCount;
    int gDevCount;
    switch(currMode)
    {
        case MODE_GG:
            gCount = groupGList.size();//сколько географических районов занесено в базе
            for (int i=0;i<gCount;i++)
            {
                //fill device list
                gDevCount = groupGList.at(i)->deviceList.size();
                for (int j=0;j<gDevCount;j++)
                {
                    groupGList.at(i)->gw->AddDevice(groupGList.at(i)->deviceList.at(j)->getButton());
                    groupGList.at(i)->deviceList.at(j)->setCol();
                    groupGList.at(i)->deviceList.at(j)->updateMainIcon();
                }
                groupGList.at(i)->gw->setMonitorMode(MODE_GG);
                groupGList.at(i)->gw->setGID(groupGList.at(i)->index);
                ui.tabWidget->addTab(groupGList.at(i)->gw,groupGList.at(i)->name);
                groupGList.at(i)->gw->key_group=option.group;
                //groupGList.at(i)->gw->key_circlecommand=option.circlecommand;
                groupGList.at(i)->gw->setButED();
            }
            break;
        case MODE_GA:
            SortDevices(deviceList,MODE_GA);
            gCount = groupAList.size();
            for (int i=0;i<gCount;i++){
                //fill device list
                gDevCount = groupAList.at(i)->deviceList.size();
                for (int j=0;j<gDevCount;j++){
                    groupAList.at(i)->gw->AddDevice(groupAList.at(i)->deviceList.at(j)->getButton());
                    groupAList.at(i)->deviceList.at(j)->setCol();
                    groupAList.at(i)->deviceList.at(j)->updateMainIcon();
                }
                groupAList.at(i)->gw->setMonitorMode(MODE_GA);
                groupAList.at(i)->gw->setGID(groupAList.at(i)->index);
                ui.tabWidget->addTab(groupAList.at(i)->gw,groupAList.at(i)->name);
                groupAList.at(i)->gw->key_group=option.group;
                //groupAList.at(i)->gw->key_circlecommand=option.circlecommand;
                groupAList.at(i)->gw->setButED();
            }
            break;
        case MODE_STATE:
            gCount = groupSList.size();
            for (int i=0;i<gCount;i++){
                //fill device list
                gDevCount = groupSList.at(i)->deviceList.size();
                for (int j=0;j<gDevCount;j++){
                    groupSList.at(i)->gw->AddDevice(groupSList.at(i)->deviceList.at(j)->getButton());
                    groupSList.at(i)->deviceList.at(j)->setCol();
                    groupSList.at(i)->deviceList.at(j)->updateMainIcon();
                }
                
                groupSList.at(i)->gw->setMonitorMode(MODE_STATE);
                groupSList.at(i)->gw->setGID(groupSList.at(i)->index);
                ui.tabWidget->addTab(groupSList.at(i)->gw,groupSList.at(i)->name);
                groupSList.at(i)->gw->key_group=option.group;
                //groupSList.at(i)->gw->key_circlecommand=option.circlecommand;
                groupSList.at(i)->gw->setButED();
            }
                break;
        case MODE_COUNT:
            gCount = groupCList.size();
            for (int i=0;i<gCount;i++){
                //fill device list
                gDevCount = groupCList.at(i)->deviceList.size();
                for (int j=0;j<gDevCount;j++){
                    groupCList.at(i)->gw->AddDevice(groupCList.at(i)->deviceList.at(j)->getButton());
                    groupCList.at(i)->deviceList.at(j)->setCol();
                    groupCList.at(i)->deviceList.at(j)->updateMainIcon();
                }
                groupCList.at(i)->gw->setMonitorMode(MODE_COUNT);
                groupCList.at(i)->gw->setGID(groupCList.at(i)->index);
                ui.tabWidget->addTab(groupCList.at(i)->gw,groupCList.at(i)->name);
                groupCList.at(i)->gw->key_group=option.group;
                //groupCList.at(i)->gw->key_circlecommand=option.circlecommand;
                groupCList.at(i)->gw->setButED();
            }
            break;
        case MODE_ALL:
                //все
                gCount = groupAllList.size();
                for (int i=0;i<gCount;i++)
                {
                    gDevCount = groupAllList.at(i)->deviceList.size();
                    for (int j=0;j<gDevCount;j++)
                    {
                        groupAllList.at(i)->gw->AddDevice(groupAllList.at(i)->deviceList.at(j)->getButton());
                        groupAllList.at(i)->deviceList.at(j)->setCol();
                        groupAllList.at(i)->deviceList.at(j)->updateMainIcon();
                    }
                    groupAllList.at(i)->gw->setMonitorMode(MODE_ALL);
                    ui.tabWidget->addTab(groupAllList.at(i)->gw,tr("Все приборы"));
                    groupCList.at(i)->gw->setGID(groupAllList.at(i)->index);
                    groupAllList.at(i)->gw->key_group=option.group;
                    //groupAllList.at(i)->gw->key_circlecommand=option.circlecommand;
                    groupAllList.at(i)->gw->setButED();
                }
                break;
        default:
                /*sorting_mode=bs->settings->value("sort/all",sorting_mode).toInt();
                if (!sorting_mode) 
                {
                    sorting_mode=1;
                    bs->settings->setValue("sort/all",sorting_mode);
                }*/
            break;
    }
    if(ui.tabWidget->count()!=0 && ui.tabWidget->count()>cur_position)
        ui.tabWidget->setCurrentIndex(cur_position);
    else
        ui.tabWidget->setCurrentIndex(0);
    //ui.label_SortingMode->hide();
    //ui.cb_SortingMode->hide();
    ui.tabWidget->show();
    
    /*
    //delete all present widgets
    ui.tabWidget->hide();
    while (ui.tabWidget->count() != 0){
        QWidget* tW =  ui.tabWidget->widget(0);
        ui.tabWidget->removeTab(0);
        delete tW;
    }
    //select Groups and add widgets in created tabs with params
    QSqlQuery *tq = NULL;
    int j=0;
    GroupWidget *tGW;
    switch(currMode)
    {
    case MODE_GG:
        tq = new QSqlQuery("select rec_id, name from groups_g");
        //break; Тут умышленно нету брейка!!!!!!!!!!!!!!
    case MODE_GA:
        if (!tq){
            tq = new QSqlQuery("select rec_id, name from avtonom_groups");
        }
        //а нету брейка бо тут обработка или первого или второго запроса
        if (tq->isActive()){
            while (tq->next()){
                tGW = new GroupWidget(NULL,currMode,tq->value(0).toInt(),j,tq->value(1).toString());
                connect(tGW,SIGNAL(toServer()),this,SLOT(signalToServer()));
                connect(tGW,SIGNAL(devCountChanged(int, QString)),this,SLOT(onDevCountChanged(int, QString)));
                ui.tabWidget->addTab(tGW,tq->value(1).toString());
                //tGW->updateFromBD();
                j++;
            }
        }
        break;
    case MODE_STATE:
        tGW = new GroupWidget(NULL,currMode,0,0,tr("Ручной - ОК"));
        connect(tGW,SIGNAL(toServer()),this,SLOT(signalToServer()));
        connect(tGW,SIGNAL(devCountChanged(int, QString)),this,SLOT(onDevCountChanged(int, QString)));
        ui.tabWidget->addTab(tGW,tr("Ручной - ОК"));
        //tGW->updateFromBD();
        tGW = new GroupWidget(NULL,currMode,1,1,tr("Авто - ОК"));
        connect(tGW,SIGNAL(toServer()),this,SLOT(signalToServer()));
        connect(tGW,SIGNAL(devCountChanged(int, QString)),this,SLOT(onDevCountChanged(int, QString)));
        ui.tabWidget->addTab(tGW,tr("Авто - ОК"));
    //	tGW->updateFromBD();
        tGW = new GroupWidget(NULL,currMode,2,2,tr("Ручной - Авария"));
        connect(tGW,SIGNAL(toServer()),this,SLOT(signalToServer()));
        connect(tGW,SIGNAL(devCountChanged(int, QString)),this,SLOT(onDevCountChanged(int, QString)));
        ui.tabWidget->addTab(tGW,tr("Ручной - Авария"));
        //tGW->updateFromBD();
        tGW = new GroupWidget(NULL,currMode,3,3,tr("Авто - Авария"));
        connect(tGW,SIGNAL(toServer()),this,SLOT(signalToServer()));
        connect(tGW,SIGNAL(devCountChanged(int, QString)),this,SLOT(onDevCountChanged(int, QString)));
        ui.tabWidget->addTab(tGW,tr("Авто - Авария"));
        //tGW->updateFromBD();
        break;
    default:
        break;
    }
    if (tq) delete tq;
    ui.tabWidget->show();
    updateMonitor();*/
    
}

void monitor::updateMonitor(){
    
    /*int tabCount = ui.tabWidget->count();
    for (int i=0;i<tabCount;i++){
        ((GroupWidget*)ui.tabWidget->widget(i))->updateFromBD();
    }*/
    if (informationWindow->isVisible()){
        informationWindow->drawSHUNO(informationWindow->master->rec_id);
    }
    
}

void monitor::onDevCountChanged(int tabId, QString tabString){
    //ui.tabWidget->setTabText(tabId,tabString);
}

bool monitor::makeConnection(){
    BaseSettings *bs = BaseSettings::createInstance();
    
    if(bs->settings->value("sort/sort_g","1").toInt()==1)
        sort_g=true;
    else
        sort_g=false;

    if(bs->settings->value("sort/sort_a","1").toInt()==1)
        sort_a=true;
    else
        sort_a=false;
    
    QString host = bs->settings->value("Connection/server","192.168.0.1").toString();
    QString dbname = bs->settings->value("Connection/dbname","gorsvet").toString();;
    QString dbuser = bs->settings->value("Connection/dbuser","monitor").toString();;
    QString dbpassword = bs->settings->value("Connection/dbpassword","monitor").toString();
    int dbport = bs->settings->value("Connection/dbport","3306").toInt();
    workDB = QSqlDatabase::addDatabase("QMYSQL");
    workDB.setHostName(host);
    workDB.setPort(dbport);
    workDB.setDatabaseName(dbname);
    workDB.setUserName(dbuser);
    workDB.setPassword(dbpassword);
    QString dbVer;
    bool retval=false;
    /*if(Security())
    {*/
        if (workDB.open())
        {
            retval =  TRUE;
            QSqlQuery tq;
            tq.exec("SET NAMES utf8");
            //VERSION
            if (!tq.exec("select * from ver")){
                QMessageBox::critical(0, qApp->tr("Ошибка"),
                    tr("Отсутствует версия базы. Обратитесь в Одис-W."), QMessageBox::Ok,
                    QMessageBox::NoButton);
                retval =  FALSE;
            }else{//версия есть
                if (tq.next()){
                    dbVer = tq.value(tq.record().indexOf("major")).toString()+"."+
                        tq.value(tq.record().indexOf("minor")).toString()+"."+
                        tq.value(tq.record().indexOf("build")).toString();

                    if ((tq.value(tq.record().indexOf("major")).toUInt() == 2)//&&//это
                        /*(tq.value(tq.record().indexOf("minor")).toUInt() == 9)&&//же
                        (tq.value(tq.record().indexOf("build")).toUInt() == 0)*/)//бред!
                        {
                            //USER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            QString loginName = bs->settings->value("Connection/default_user","user").toString();
                            tq.prepare("select user_role from user where name=:name and passwd=PASSWORD(:passwd)");
                            tq.bindValue(":name",loginName);
                            tq.bindValue(":passwd","");
                            if (!tq.exec(/*"select user_role from user where name='admin' and passwd=PASSWORD('admin')"*/)){
                                QSqlError err = workDB.lastError();
                                QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
                                    err.text(), QMessageBox::Cancel,
                                    QMessageBox::NoButton);
                                retval =  FALSE;
                            }else{
                                if (tq.next()){
                                    int userRole = tq.value(tq.record().indexOf("user_role")).toUInt();
                                    bs->accessLevel = userRole;//(!!!убрать это говно срочно!!!)
                                    if(userRole>1)
                                    {
                                        informationWindow->key_login=true;
                                        key_option=true;
                                    }
                                    /*if(userRole>2)
                                    {
                                        key_login=true;
                                        key_option=true;
                                    }*/
                                }else{
                                    //////////////////////////////////////////////////////////////////////////
                                    //////////////////////////////////////////////////////////////////////////
                                    //показать окно
                                    bool enaught = FALSE;
                                    QString passwd;
                                    while (!enaught){
                                        bool ok;
                                        loginName = QInputDialog::getText(this, tr("Введите логин"),
                                            tr("Логин:"), QLineEdit::Normal,
                                            "admin", &ok);
                                        if (ok && !loginName.isEmpty()){
                                            passwd = QInputDialog::getText(this, tr("Введите пароль"),
                                                tr("Пароль:"), QLineEdit::Password,
                                                "", &ok);
                                            if (ok){
                                                tq.prepare("select user_role from user where name=:name and passwd=PASSWORD(:passwd)");
                                                tq.bindValue(":name",loginName);
                                                tq.bindValue(":passwd",passwd);
                                                if (!tq.exec()){
                                                    QSqlError err = workDB.lastError();
                                                    QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
                                                        err.text(), QMessageBox::Cancel,
                                                        QMessageBox::NoButton);
                                                    retval =  FALSE;
                                                    enaught = TRUE;
                                                }else{
                                                    if (tq.next()){
                                                        int userRole = tq.value(tq.record().indexOf("user_role")).toUInt();
                                                        bs->accessLevel = userRole;
                                                        if(userRole>1)
                                                        {
                                                            informationWindow->key_login=true;
                                                            key_option=true;
                                                        }
                                                        /*if(userRole>2)
                                                        {
                                                            key_login=true;
                                                            key_option=true;
                                                        }*/
                                                        enaught = TRUE;
                                                    }
                                                }
                                            }else{
                                                retval = FALSE;
                                                enaught = TRUE;
                                            }
                                        }else{
                                            retval = FALSE;
                                            enaught = TRUE;
                                        }
                                        //получить логин и пароль
                                        //попытаться законектиться
                                    }
                                    //////////////////////////////////////////////////////////////////////////
                                    //////////////////////////////////////////////////////////////////////////
                                }
                            }
                    }else{
                        dbVer.prepend(tr("Не верная версия БД ("));
                        dbVer.append(").\n");
                        dbVer.append(tr("Обновите."));
                        QMessageBox::critical(0, qApp->tr("Ошибка версии"),
                            dbVer, QMessageBox::Ok,
                            QMessageBox::NoButton);
                        retval =  FALSE;
                    }
                }else{
                    QMessageBox::critical(0, qApp->tr("Ошибка"),
                        tr("Ошибка получения версии БД"), QMessageBox::Ok,
                        QMessageBox::NoButton);
                    retval =  FALSE;
                }
            }
        }else{
            QSqlError err = workDB.lastError();
            QMessageBox::critical(0, qApp->tr("Невозможно подключиться к БД"),
                err.text(), QMessageBox::Cancel,
                QMessageBox::NoButton);
            retval =  FALSE;
        }
        bs->freeInst();
    /*}
    else
    {
        QMessageBox::critical(0, qApp->tr("Ошибка"),tr("  Некорректный запуск приложения.\n Обратитесь в службу тех. поддержки!"), QMessageBox::Ok,QMessageBox::NoButton);
        retval=false;
    }*/
    return retval;
}

void monitor::on_pbGG_clicked()
{
    mode_sort=0;
    key_widg=true;
    if (currMode != MODE_GG){
        currMode = MODE_GG;
        //refreshModeButtons(ui.pbGG);
        updateAfterConfig();
    }
    if (!ui.pbGG->isChecked()){
        ui.pbGG->setChecked(TRUE);
    }
    
}

void monitor::on_pbGA_clicked()
{
    mode_sort=1;
    key_widg=true;
    if (currMode != MODE_GA){
        currMode = MODE_GA;
        //refreshModeButtons(ui.pbGA);
        updateAfterConfig();
    }
    if (!ui.pbGA->isChecked()){
        ui.pbGA->setChecked(TRUE);
    }
    
    
}

void monitor::on_pbState_clicked()
{
    mode_sort=2;
    key_widg=true;
    if (currMode != MODE_STATE){
        currMode = MODE_STATE;
        //refreshModeButtons(ui.pbState);
        updateAfterConfig();
    }
    if (!ui.pbState->isChecked()){
        ui.pbState->setChecked(TRUE);
    }
    
    
}
void monitor::on_pbCount_clicked()
{
    mode_sort=3;
    key_widg=true;
    if (currMode != MODE_COUNT){
        currMode = MODE_COUNT;
        //refreshModeButtons(ui.pbState);
        updateAfterConfig();
    }
    if (!ui.pbCount->isChecked()){
        ui.pbCount->setChecked(TRUE);
    }
}
void monitor::on_pb_alldevice_clicked()
{
    mode_sort=4;
    key_widg=true;
    if (currMode != MODE_ALL){
        currMode = MODE_ALL;
        //refreshModeButtons(ui.pbGG);
        updateAfterConfig();
    }
    if (!ui.pb_alldevice->isChecked()){
        ui.pb_alldevice->setChecked(TRUE);
    }
    
}
void monitor::getMap(int dev_id)
{
    SignaltoMap(dev_id);
}

void monitor::getCommand(int dev_id, QString in_cmd, int in_cmd_type)
{
    QSqlQuery *tq = new QSqlQuery();
    
    /*tq->prepare("insert into send (device_id, command, time_seting, time_send, command_state, command_type) "
        "values (:device_id, :command, NOW(), NOW(), :command_state, :command_type)"
        );
    tq->bindValue(":device_id",dev_id);
    tq->bindValue(":command",in_cmd);
    tq->bindValue(":command_state",12);
    tq->bindValue(":command_type",in_cmd_type);
    if (!tq->exec()){

    }*/

    QString str;
    str.clear();
    
    str=str+"values ("+QString::number(dev_id,10)+", '"+in_cmd+"', "+"NOW(), "+"NOW(), "+"12, "+QString::number(in_cmd_type,10)+")";
    if (!tq->exec("insert into send (device_id, command, time_seting, time_send, command_state, command_type) "+str))
    {
        fprintf(stdout,"\nError insert database [1607]...");
    }
    delete tq;
    signalToServer();
}

void monitor::signalToServer(){
    if (soc->isOpen())
    {
        if (soc->write("READ_BASE\n") == -1)
        {
            QMessageBox::critical(0, qApp->tr("Ошибка связи с сервером"),
                tr("Ошибка соединения управляющего канала 1.\n Будет произведена попытка нового подключения."), QMessageBox::Ok,
                QMessageBox::NoButton);
            connectToServer();
            if (soc->isOpen())
            {
                soc->write("READ_BASE\n");
            }
        }
    }
    else
    {
        QMessageBox::critical(0, qApp->tr("Ошибка связи с сервером"),
            tr("Ошибка соединения управляющего канала.\n Будет произведена попытка нового подключения."), QMessageBox::Ok,
            QMessageBox::NoButton);
        connectToServer();
        if (soc->isOpen())
        {
            soc->write("READ_BASE\n");
        }
    }
    updateMonitor();
}


/*void monitor::signalAllToServer()
{
    QString str;

    str.clear();
    str.append("READ_BASE#"+COMM_STR+"\n");
    
    if (soc->isOpen())
    {
        if (soc->write(str.toAscii().constData()) == -1)
        {
            QMessageBox::critical(0, qApp->tr("Ошибка связи с сервером"),
                tr("Ошибка соединения управляющего канала.\n Будет произведена попытка нового подключения."), QMessageBox::Ok,
                QMessageBox::NoButton);
            connectToServer();
            if (soc->isOpen())
            {
                soc->write(str.toAscii().constData());
            }
        }
    }
    else
    {
        QMessageBox::critical(0, qApp->tr("Ошибка связи с сервером"),
            tr("Ошибка соединения управляющего канала.\n Будет произведена попытка нового подключения."), QMessageBox::Ok,
            QMessageBox::NoButton);
        connectToServer();
        if (soc->isOpen())
        {
            soc->write(str.toAscii().constData());
        }
    }
    updateMonitor();
}*/

void monitor::SignaltoMap(int dev_id)
{
    QString str;
    str="MONITOR_UPDATED:"+QString::number(dev_id,10)+"\n";
    if (soc->isOpen())
    {
        // old if (soc->write(str.toAscii().constData()) == -1)
        if(soc->write(str.toUtf8().constData()) == -1)
        {
            QMessageBox::critical(0, qApp->tr("Ошибка связи с сервером"),
                tr("Ошибка соединения управляющего канала.\n Будет произведена попытка нового подключения."), QMessageBox::Ok,
                QMessageBox::NoButton);
            connectToServer();
            if (soc->isOpen())
                soc->write(str.toUtf8().constData());
                // old soc->write(str.toAscii().constData());
        }
    }
}

void monitor::ontimer_ver()
{
    timer_ver.stop();
    QString str;
    QString aboutText=QString("Programm Monitor - version unknown, build date: %1, %2\n").arg(__DATE__).arg(__TIME__);
    //добавить версию
    str="ABOUT: "+aboutText;
    if (soc->isOpen())
        soc->write(str.toUtf8().constData());
        // old soc->write(str.toAscii().constData());
}

void monitor::connectToServer()
{
    soc->connectToHost(mainBs->settings->value("Connection/app_server","192.168.0.1").toString(),
        mainBs->settings->value("Connection/app_port","192.168.0.1").toInt(),
        QIODevice::ReadWrite);
    timer_ver.start();
}

void monitor::UpdateDB()
{

    QString str;
    str.append("UPDATED_DATABASE:"+QString::number(BD_id,10)+"\n");
    if (soc->isOpen())
    {
        // old if (soc->write(str.toAscii().constData()) == -1)
        if(soc->write(str.toUtf8().constData()) == -1)
        {
            QMessageBox::critical(0, qApp->tr("Ошибка связи с сервером"),
                tr("Ошибка соединения управляющего канала.\n Будет произведена попытка нового подключения."), QMessageBox::Ok,
                QMessageBox::NoButton);
            connectToServer();
            if (soc->isOpen())
            {
                soc->write("UPDATED_DATABASE:\n");
            }
        }
    }
}
void monitor::onSocketConnected(){
    mainBs->connectedToApp = TRUE;
}

void monitor::onSocketDisconnected(){
    mainBs->connectedToApp = FALSE;
}

void monitor::onSocketError(QAbstractSocket::SocketError socErr){
    QString txtErr;
    switch(socErr)
    {
    case QAbstractSocket::ConnectionRefusedError:
        //txtErr = tr("The connection was refused by the peer (or timed out).");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        //txtErr = tr("The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::HostNotFoundError:
        //txtErr = tr("The host address was not found.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::SocketAccessError:
        //txtErr = tr("The socket operation failed because the application lacked the required privileges.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::SocketResourceError:
        //txtErr = tr("The local system ran out of resources (e.g., too many sockets).");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::SocketTimeoutError:
        //txtErr = tr("The socket operation timed out.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::DatagramTooLargeError:
        //txtErr = tr("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::NetworkError:
        //txtErr = tr("An error occurred with the network (e.g., the network cable was accidentally plugged out).");
        break;
    case QAbstractSocket::AddressInUseError:
        //txtErr = tr("The address specified to QUdpSocket::bind() is already in use and was set to be exclusive.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        //txtErr = tr("The address specified to QUdpSocket::bind() does not belong to the host.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        //txtErr = tr("The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        //txtErr = tr("The socket is using a proxy, and the proxy requires authentication.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    case QAbstractSocket::UnknownSocketError:
        //txtErr = tr("An unidentified error occurred.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    default:
        //txtErr = tr("An unidentified error occurred.");
        txtErr = tr("Отсутствует связь с сервером! Ошибка соединения управляющего канала!");
        break;
    }
    QMessageBox::critical(0, qApp->tr("Ошибка системы"),
        txtErr, QMessageBox::Cancel,
        QMessageBox::NoButton);
}


void monitor::parser(QString incoming){
    int oldErrorState;
    IPObject *tO;
    incoming = incoming.toUpper();
    QRegExp shablon;
    shablon.setPatternSyntax(QRegExp::Wildcard);
    QStringList parcedByDoubleComa = incoming.split(":");
    

    //DATABASE_UPDATE
    shablon.setPattern("*UPD_DATABASE:*");
    if (shablon.exactMatch(incoming))
    {
        if(parcedByDoubleComa.at(1).toInt()!=BD_id)
        {
            bool key=false;
            if(gui_TerminalsEdit->isVisible())
            {
                gui_TerminalsEdit->hide();
                key=true;
            }
            deleteGroupsList();
            deviceList.clear();
            buildDeviceList();
            buildGroupsList();
            if(key)
                gui_TerminalsEdit->iShow();

        }
    }


    //REGISTER
    shablon.setPattern("*REGISTERED:*");
    if (shablon.exactMatch(incoming)){

    }
    //LAVEL_UPDATED
    shablon.setPattern("*LAVEL_UPDATED:*");
    if (shablon.exactMatch(incoming))
    {
        if (parcedByDoubleComa.size() >= 2)
        {
            QStringList parcedlavel = parcedByDoubleComa.at(2).split(",");
            if (informationWindow->isVisible())
            {
                if(parcedlavel.at(0).contains(informationWindow->master->deviceDescr.phone,Qt::CaseInsensitive))
                {
                    informationWindow->master->lavel=parcedlavel.at(1).toInt();
                    informationWindow->count_anim=0;
                    informationWindow->key_anim=true;
                    informationWindow->key_hisgsm=true;
                    updateMonitor();
                }
            }
        }
    }

    
    //DATABASE_UPDATED
    //
    shablon.setPattern("*DATABASE_UPDATED:*");
    if (shablon.exactMatch(incoming)){
        //get updated index
        if (parcedByDoubleComa.size() == 2){
            int dev_index = parcedByDoubleComa.at(1).toInt();
            int dev_count = deviceList.size();
            for (int i=0;i<dev_count;i++){
                if (deviceList.at(i)->rec_id == dev_index)
                {
                    logparser(incoming);
                    //update state
                    oldErrorState = deviceList.at(i)->getGroupS();
                    deviceList.at(i)->updateDevice();
                    deviceList.at(i)->updateState();
                    deviceList.at(i)->updateMainIcon();
                    deviceList.at(i)->updateImages();
                    if (oldErrorState != deviceList.at(i)->getGroupS()){
                        //REMOVE
                        for (int j=0;j<groupSList.at(oldErrorState)->deviceList.count();j++){
                            if (groupSList.at(oldErrorState)->deviceList.at(j)->rec_id == dev_index){
                                tO = groupSList.at(oldErrorState)->deviceList.takeAt(j);
// 								if (currMode == MODE_STATE){
// 									groupSList.at(oldErrorState)->gw->DelDevice(tO->getButton());
// 								}
                                break;
                            }
                        }
                        //ADD
                        groupSList.at(deviceList.at(i)->getGroupS())->deviceList.append(tO);
                        if (currMode == MODE_STATE){
                            groupSList.at(deviceList.at(i)->getGroupS())->gw->AddDevice(tO->getButton());
                        }
                    }
                    i = 0xFFFF;
                }
            }
        }
        updateMonitor();
        return;
    }
    //SYSTEM_ERROR:
    /*shablon.setPattern("*SYSTEM_ERROR:*");
    if (shablon.exactMatch(incoming)){
        QMessageBox::critical(0, qApp->tr("Ошибка сервера"),
            qApp->tr("Сервер сообщил о регистрации системного сбоя."), QMessageBox::Cancel,
            QMessageBox::NoButton);
        return;
    }*/
    //MODEM_ERROR:
    /*shablon.setPattern("*MODEM_ERROR: *");
    if (shablon.exactMatch(incoming)){
        QMessageBox::critical(0, qApp->tr("Сбой подсистемы модемов"),
            incoming, QMessageBox::Cancel,
            QMessageBox::NoButton);
        return;
    }*/
    //BEEP
    //MODEM_ERROR:
    shablon.setPattern("*BEEP*");
    if (shablon.exactMatch(incoming)){
        //beep ))
        QSound::play("ringin.wav");
        return;
    }
    //RESTARTED_SYSTEM
    shablon.setPattern("*RESTARTED_SYSTEM:*");
    if (shablon.exactMatch(incoming))
    {
        QString str;
        QStringList arguments;
        QMessageBox msgBox;

        str.clear();
        QSound::play("ringin.wav");
        
        str.append(tr("Сервер сообщил о регистрации системного сбоя. \nНеобходим перезапуск системы. \n\n\n Выполнить перезагрузку компьютера?"));
        // Заголовок сообщения
        msgBox.setText(tr("Ошибка сервера")); 
        // Тип иконки сообщения
        msgBox.setIcon(QMessageBox::Information); 
        // Основное сообщение Message Box
        msgBox.setInformativeText(str); 
        // Добавление реагирования на софт клавиши
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel); 
        // На какой кнопке фокусироваться по умолчанию
        msgBox.setDefaultButton(QMessageBox::Ok); 
        /* Запускаем QMessageBox. После выполнения, в ret будет лежать значение кнопки, на которую нажали - это необходимо для дальнейшей обработки событий*/
        msgBox.setIcon(QMessageBox::Critical);
        int ret = msgBox.exec(); 
        // Собственно вот этот case и отвечает за обработку событий
        switch (ret)
        {   
        case QMessageBox::Cancel:
             // Сюда пишем обработку события Cancel
            return;
             break;
          case QMessageBox::Ok:
             // Сюда пишем обработку события Ok
            str=QCoreApplication::applicationDirPath();
            str.replace("/","\\");
            str=str+"\\Install_dotnet.exe";

            process_ext=new QProcess();
            arguments << "-d";
            //process_ext->start(str,arguments);
            process->startDetached(str,arguments);
            return;
             break;
          default:
             // Сюда пишем обработку события по умолчанию
              return;
             break;
        }
    }
}


void monitor::onDataReady(){
    char tCh;
    int bytesCount = soc->bytesAvailable();
    if (bytesCount>0){
        for (bytesCount;bytesCount>0;bytesCount--){
            if (soc->getChar(&tCh)){
                //ok
                readBuffer.append(tCh);
                if (tCh == 10){
                    //Entr found - parsing
                    readBuffer = readBuffer.simplified();
                    parser(readBuffer);
                    readBuffer.clear();
                }
            }else{
                //error getch
            }
        }
    }

}
void monitor::clearDeviceList()
{
    //clear list
    while (!deviceList.isEmpty())
    {
        delete deviceList.takeFirst();
    }
}


void monitor::buildDeviceList()
{
    //fill list
    progress = new PrBar(NULL);
    QSqlQuery *tq = new QSqlQuery("select rec_id from device order by position");
    //QSqlQuery *tq = new QSqlQuery("select rec_id from device order by name desc");
    /*QSqlQuery *tempq = new QSqlQuery("SET GLOBAL log_bin_trust_function_creators=1;\n"
"DROP FUNCTION IF EXISTS digits;\n"
"DELIMITER |\n"
"CREATE FUNCTION digits( str CHAR(32) ) RETURNS CHAR(32)\n"
"BEGIN\n"
  "DECLARE i, len SMALLINT DEFAULT 1;\n"
  "DECLARE ret CHAR(32) DEFAULT '';\n"
  "DECLARE c CHAR(1);\n"
  "SET len = CHAR_LENGTH( str );\n"
  "REPEAT\n"
    "BEGIN\n"
      "SET c = MID( str, i, 1 );\n"
      "IF c BETWEEN '0' AND '9' THEN \n"
        "SET ret=CONCAT(ret,c);\n"
      "END IF;\n"
      "SET i = i + 1;\n"
    "END;\n"
  "UNTIL i > len END REPEAT;\n"
  "RETURN ret;\n"
"END |\n"
"DELIMITER ;");

    if ( ! tempq->isActive() )
    QMessageBox::warning(
        this, tr("Database Error"),
        tempq->lastError().text() );

    if (tempq->isActive()){
        while (tempq->next()){
        }}
QSqlQuery *tq = new QSqlQuery("SELECT `rec_id` FROM `device` ORDER BY digits(name)+0");*/
    progress->setBarName(tr("Построение списка ИП"));
    progress->setMaxVal(tq->size());
    progress->setVal(0);
    progress->iShow();
    int i=0;
    deviceList.clear();
    if (tq->isActive()){
        while (tq->next()){
            IPObject *tO = new IPObject(this,tq->value(0).toInt());
            //connect(tO,SIGNAL(deviceToIPW(IPObject *)),informationWindow,SLOT(iShow(IPObject *)));

            deviceList.append(tO);
            //QRegExp rx("\d+");
            //int pos_nameNumber=rx.indexIn(((*tO).deviceDescr).name );
            progress->setBarName(tr("Построение списка ИП: ") + ((*tO).deviceDescr).name );
            progress->setVal(i);
            i++;
            QApplication::processEvents();
        }
    }
    //SortDevices(deviceList,MODE_ALL);
    progress->hide();
    delete progress;
    delete tq;
}


void monitor::deleteGroupsList(){
    //clear all
    //GList
    while (!groupGList.isEmpty()){
        GrList *tl = groupGList.takeFirst();
        tl->deviceList.clear();
        delete tl;
    }
    //AList
    while (!groupAList.isEmpty()){
        GrList *tl = groupAList.takeFirst();
        tl->deviceList.clear();
        delete tl;
    }
    //SList
    while (!groupSList.isEmpty()){
        GrList *tl = groupSList.takeFirst();
        tl->deviceList.clear();
        delete tl;
    }
    while (!groupCList.isEmpty()){
        GrList *tl = groupCList.takeFirst();
        tl->deviceList.clear();
        delete tl;
    }
    while (!groupAllList.isEmpty()){
        GrList *tl = groupAllList.takeFirst();
        tl->deviceList.clear();
        delete tl;
    }
}

void monitor::buildGroupsList(){
    
    //create groups
    GrList *tl;
    int devCount = deviceList.size();
    QSqlQuery *tq = new QSqlQuery();
    //all device
    tl = new GrList;
    tl->index =0;
    tl->name = tr("Все приборы");
    tl->gw = new GroupWidget(NULL);
    for (int i=0;i<devCount;i++)
    {
        tl->deviceList.append(deviceList.at(i));
    }
    SortDevices(tl->deviceList,MODE_ALL);
    connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
    connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
    //connect(tl->gw,SIGNAL( ()),this,SLOT(signalToServer()));
    //connect(tl->gw,SIGNAL(toAllServer()),this,SLOT(signalAllToServer()));
    tl->gw->setParent(this);
    groupAllList.append(tl);
    //GList - select from groupsG
    if(sort_g)
        tq->exec("select rec_id, name, human_descr from	groups_g order by name");
    else
        tq->exec("select rec_id, name, human_descr from	groups_g order by name desc");
    if (tq->isActive()){
        while (tq->next()){
            tl = new GrList;
            tl->index = tq->value(0).toInt();
            tl->name = tq->value(1).toString();
            tl->descr = tq->value(2).toString();
            tl->gw = new GroupWidget(NULL);
            //fill device list
            for (int i=0;i<devCount;i++)
            {
                if (deviceList.at(i)->getGroupG() == tl->index)
                {
                    //наш клиент
                    tl->deviceList.append(deviceList.at(i));
                    //tl->gw->AddDevice(deviceList.at(i)->getButton());
                }
                
            }
            SortDevices(tl->deviceList,MODE_GG);
            //
            connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
            connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
            //connect(tl->gw,SIGNAL(toServer()),this,SLOT(signalToServer()));
            tl->gw->setParent(this);
            groupGList.append(tl);
        }
    }
    //AList - select from groupsA
    if(sort_a)
        tq->exec("select rec_id, name, human_descr from	avtonom_groups order by name");
    else
        tq->exec("select rec_id, name, human_descr from	avtonom_groups order by name desc");

    if (tq->isActive()){
        while (tq->next()){
            tl = new GrList;
            tl->index = tq->value(0).toInt();
            tl->name = tq->value(1).toString();
            tl->descr = tq->value(2).toString();
            tl->gw = new GroupWidget(NULL);
            //fill device list
            for (int i=0;i<devCount;i++){
                if (deviceList.at(i)->getGroupA() == tl->index){
                    //наш клиент
                    tl->deviceList.append(deviceList.at(i));
                    //tl->gw->AddDevice(deviceList.at(i)->getButton());
                }
            }
            SortDevices(tl->deviceList,MODE_GA);

            //
            connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
            connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
            //connect(tl->gw,SIGNAL(toServer()),this,SLOT(signalToServer()));
            //connect(tl->gw,SIGNAL(toAllServer()),this,SLOT(signalAllToServer()));
            tl->gw->setParent(this);
            //!!SortDevices(tl->deviceList,MODE_GA);
            groupAList.append(tl);
        }
    }
    //State groups
    //for normal
    tl = new GrList;
    tl->index = 0;
    tl->name = tr("Норма");
    tl->descr = tr("Приборы без аварии");
    tl->gw = new GroupWidget(NULL);
    for (int i=0;i<devCount;i++){
        if (deviceList.at(i)->getGroupS() == tl->index){
            //наш клиент
            tl->deviceList.append(deviceList.at(i));
            //tl->gw->AddDevice(deviceList.at(i)->getButton());
        }
    }
    connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
    connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
    //connect(tl->gw,SIGNAL(toServer()),this,SLOT(signalToServer()));
    //connect(tl->gw,SIGNAL(toAllServer()),this,SLOT(signalAllToServer()));
    tl->gw->setParent(this);
    //SortDevices(tl->deviceList,MODE_GA);
    groupSList.append(tl);
    //for ERROR
    tl = new GrList;
    tl->index = 1;
    tl->name = tr("Авария");
    tl->descr = tr("Приборы с аварией");
    tl->gw = new GroupWidget(NULL);
    for (int i=0;i<devCount;i++){
        if (deviceList.at(i)->getGroupS() == tl->index){
            //наш клиент
            tl->deviceList.append(deviceList.at(i));
            //tl->gw->AddDevice(deviceList.at(i)->getButton());
        }
    }
    connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
    connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
    //connect(tl->gw,SIGNAL(toServer()),this,SLOT(signalToServer()));
    //connect(tl->gw,SIGNAL(toAllServer()),this,SLOT(signalAllToServer()));
    tl->gw->setParent(this);
    groupSList.append(tl);
    //create widgets
    delete tq;

    tl = new GrList;
    tl->index = 0;
    tl->name = tr("Приборы с счётчиками");
    tl->gw = new GroupWidget(NULL);
    for (int i=0;i<devCount;i++)
    {	if (deviceList.at(i)->deviceDescr.counter_type>-1)//наш клиент
        {tl->deviceList.append(deviceList.at(i));}}
    connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
    connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
    //connect(tl->gw,SIGNAL(toServer()),this,SLOT(signalToServer()));
    //connect(tl->gw,SIGNAL(toAllServer()),this,SLOT(signalAllToServer()));
    tl->gw->setParent(this);
    groupCList.append(tl);

    tl = new GrList;
    tl->index = 1;
    tl->name = tr("Приборы без счётчиков");
    tl->gw = new GroupWidget(NULL);
    for (int i=0;i<devCount;i++)
    {	if (!(deviceList.at(i)->deviceDescr.counter_type>-1))//наш клиент
        {tl->deviceList.append(deviceList.at(i));}}
    connect(tl->gw,SIGNAL(clearAhtung(MonitorShowMode, int)),this,SLOT(clearAhtungState(MonitorShowMode, int)));
    connect(tl->gw,SIGNAL(putCommand(int, QString, int)),this,SLOT(getCommand(int, QString, int)));
    //connect(tl->gw,SIGNAL(toServer()),this,SLOT(signalToServer()));
    //connect(tl->gw,SIGNAL(toAllServer()),this,SLOT(signalAllToServer()));
    tl->gw->setParent(this);
    groupCList.append(tl);
}

QString monitor::getNametoInt(QString str)
{
    bool key=false;
    uint rez=-1;
    QString s;	
    int w=-1;
    for(int y=0;y<str.size();y++)
    {
        rez=str.at(y).unicode();
        if(rez-48>=0 && rez-48<=9)
        {
            s.append(rez);
            key=true;
        }
    }
    
    if(key)
        return s;
    else
        return str;
}

void monitor::clearAhtungState(MonitorShowMode in_mode, int in_grID){
    int gCount;
    switch(in_mode)
    {
    case MODE_GG:
        gCount = groupGList.size();
        for (int i=0;i<gCount;i++){
            if (groupGList.at(i)->index == in_grID){
                for (int j=0;j<groupGList.at(i)->deviceList.count();j++){
                    groupGList.at(i)->deviceList.at(j)->deviceDescr.is_viewed = TRUE;
                    groupGList.at(i)->deviceList.at(j)->updateMainIcon();
                }
            }
        }
        break;
    case MODE_GA:
        gCount = groupAList.size();
        for (int i=0;i<gCount;i++){
            if (groupAList.at(i)->index == in_grID){
                for (int j=0;j<groupAList.at(i)->deviceList.count();j++){
                    groupAList.at(i)->deviceList.at(j)->deviceDescr.is_viewed = TRUE;
                    groupAList.at(i)->deviceList.at(j)->updateMainIcon();
                }
            }
        }
        break;
    case MODE_STATE:
        gCount = groupSList.size();
        for (int i=0;i<gCount;i++){
            if (groupSList.at(i)->index == in_grID){
                for (int j=0;j<groupSList.at(i)->deviceList.count();j++){
                    groupSList.at(i)->deviceList.at(j)->deviceDescr.is_viewed = TRUE;
                    groupSList.at(i)->deviceList.at(j)->updateMainIcon();
                }
            }
        }
        break;
    case MODE_COUNT:
        gCount = groupCList.size();
        for (int i=0;i<gCount;i++){
            if (groupCList.at(i)->index == in_grID){
                for (int j=0;j<groupCList.at(i)->deviceList.count();j++){
                    groupCList.at(i)->deviceList.at(j)->deviceDescr.is_viewed = TRUE;
                    groupCList.at(i)->deviceList.at(j)->updateMainIcon();
                }
            }
        }
        break;
    case MODE_ALL:
        gCount = groupAllList.size();
        for (int i=0;i<gCount;i++)
        {
            for (int j=0;j<groupAllList.at(i)->deviceList.count();j++)
            {
                groupAllList.at(i)->deviceList.at(j)->deviceDescr.is_viewed = TRUE;
                groupAllList.at(i)->deviceList.at(j)->updateMainIcon();
            }
        }
        break;
    default:
        break;
    }
}

void monitor::setAccessLevel(){
    QList<QObject *> childrenList = findChildren<QObject *>();
    QSqlQuery tq;
    QWidget *tW;
    QAction *tA;
    int xzxz=mainBs->accessLevel;
    for (int i=0;i<childrenList.count();i++){
        if (childrenList.at(i)->objectName() != ""){
            //"monitor_Monitor_"+childrenList.at(i)->objectName() 
            QString str="select is_enabled from roles_access where name = 'monitor_Monitor_"+
                childrenList.at(i)->objectName()+"' and role = "+QString::number(mainBs->accessLevel,10);
            if (!tq.exec("select is_enabled from roles_access where name = 'monitor_Monitor_"+
                childrenList.at(i)->objectName()+
                "' and role = "+QString::number(mainBs->accessLevel,10))){
                    //error
                    QSqlError err = tq.lastError();
                    QMessageBox::critical(0, tr("Ошибка выполнения запроса"),
                        err.text(), QMessageBox::Cancel,
                        QMessageBox::NoButton);
            }else
            {
                //ok
                if (tq.next())
                {
                    tW = dynamic_cast<QWidget*>(childrenList.at(i));
                    if (tW)
                    {
                        tW->setDisabled(!tq.value(tq.record().indexOf("is_enabled")).toBool());
                    }else
                    {
                        tA = dynamic_cast<QAction*>(childrenList.at(i));
                        if (tA)
                        {
                            tA->setDisabled(!tq.value(tq.record().indexOf("is_enabled")).toBool());
                        }
                    }
                }
                if(key_option)
                {
                    action_12->setDisabled(false);
                }
                else
                {
                    action_12->setDisabled(true);
                }
            }
        }
    }
    moreButtons->setAccessLevel();
    informationWindow->setAccessLevel();
    gui_TerminalsEdit->setAccessLevel();
}

/*void monitor::sortAllList()
{
    int devCount = deviceList.size();
    QSqlQuery *tq = new QSqlQuery();
    QString s,s1;
    GrList *tl;	

    
    for(int i=0;i<groupAllList.size();i++)
    {
        tl=(GrList *)groupAllList.at(i);
        tl->deviceList.clear();
        if(option.asc)	s="SELECT rec_id,name from device order by name";
        else			s="SELECT rec_id,name from device order by name desc";

        if(tq->exec(s))
        {
            if (tq->isActive())
            {
                while (tq->next())
                {
                    for (int j=0;j<devCount;j++)
                    {
                        if (deviceList.at(j)->deviceDescr.name == tq->value(1).toString())
                        {
                            //наш клиент
                            tl->deviceList.append(deviceList.at(j));
                            break;
                        }
                    }
                }
            }
        }
    }
    delete tq;
}*/

/*void monitor::sortGlist()
{
    int devCount = deviceList.size();
    QSqlQuery *tq = new QSqlQuery();
    QString s,s1;
    GrList *tl;	

    
    for(int i=0;i<groupGList.size();i++)
    {
        tl=(GrList *)groupGList.at(i);
        tl->deviceList.clear();
        if(option.asc)
            s="SELECT rec_id,name from device where group_g = "+QString::number(tl->index,10)+" order by CAST(name AS DECIMAL)";
        else
            s="SELECT rec_id,name from device where group_g = "+QString::number(tl->index,10)+" order by CAST(name AS DECIMAL) desc";

        
        if(tq->exec(s))
        {
            if (tq->isActive())
            {
                while (tq->next())
                {
                    for (int j=0;j<devCount;j++)
                    {
                        if (deviceList.at(j)->deviceDescr.name == tq->value(1).toString())
                        {
                            //наш клиент
                            tl->deviceList.append(deviceList.at(j));
                            break;
                        }
                    }
                }
            }
        }
    }
    delete tq;
}*/

/*void monitor::sortAlist()
{
    int devCount = deviceList.size();
    QSqlQuery *tq = new QSqlQuery();
    QString s,s1;
    GrList *tl;	

    
    for(int i=0;i<groupAList.size();i++)
    {
        tl=(GrList *)groupAList.at(i);
        tl->deviceList.clear();
        if(option.asc)
            s="SELECT rec_id,name from device where group_a = "+QString::number(tl->index,10)+" order by name";
        else
            s="SELECT rec_id,name from device where group_a = "+QString::number(tl->index,10)+" order by name desc";

        
        if(tq->exec(s))
        {
            if (tq->isActive())
            {
                while (tq->next())
                {
                    for (int j=0;j<devCount;j++)
                    {
                        if (deviceList.at(j)->deviceDescr.name == tq->value(1).toString())
                        {
                            //наш клиент
                            tl->deviceList.append(deviceList.at(j));
                            break;
                        }
                    }
                }
            }
        }
    }
    delete tq;
}*/

/*void monitor::sortSlist()
{
    int devCount = deviceList.size();
    QSqlQuery *tq = new QSqlQuery();
    QString s,s1;
    GrList *tl;	

    
    for(int i=0;i<groupSList.size();i++)
    {
        tl=(GrList *)groupSList.at(i);
        tl->deviceList.clear();
        if(option.asc)
            s="SELECT rec_id,name from device where is_error = "+QString::number(i,10)+" order by name";
        else
            s="SELECT rec_id,name from device where is_error = "+QString::number(i,10)+" order by name desc";
        
        if(tq->exec(s))
        {
            if (tq->isActive())
            {
                while (tq->next())
                {
                    for (int j=0;j<devCount;j++)
                    {
                        if (deviceList.at(j)->deviceDescr.name == tq->value(1).toString())
                        {
                            //наш клиент
                            tl->deviceList.append(deviceList.at(j));
                            break;
                        }
                    }
                }
            }
        }
    }
    delete tq;
}*/

void monitor::on_pushButton_tpfind_clicked()
{
    fnddlg->iShow();
}

void monitor::onOptionsClicked()
{
    optdlg->iShow();
}

void monitor::onCountersClicked()
{
    QString str,str1;
    str.clear();

    str.clear();
    str=QCoreApplication::applicationDirPath();
    str.replace("/","\\");
    str=str+"\\Print\\Printer.exe";

    QStringList arguments;
    QProcess* pros=new QProcess();
    pros->start(str,arguments);
}

void monitor::setAccessUpdate()
{
    informationWindow->key_solut=option.solution;
    informationWindow->key_count=option.count;
    informationWindow->key_anycommand=option.anycommand;
    informationWindow->key_config=option.config;
}

void monitor::UpdateCount()
{
    if(option.groupcount)
        ui.pbCount->setDisabled(false);
    else
        ui.pbCount->setDisabled(true);
}

bool monitor::startValidater()
{
    bool key=true;
    getDB();
    
    for(int i=0;i<dump.size();i++)
    {
        if(dump.at(i).at(0)!='8' && dump.at(i).at(0)=='+')
        {
            key=false;
            break;
        }
    }
    return key;
}

void monitor::replacePhone()
{
    QSqlQuery *tq = new QSqlQuery();
    QString str;
    QString queryStr;
    
    queryStr = "ALTER TABLE device MODIFY  phone VARCHAR(13) NULL";
    if(tq->exec(queryStr))
    {
        for(int i=0;i<dump.size();i++)
        {
            str.clear();
            str="+3";
            str.append(dump.at(i));	
            int f=rec.at(i).toInt();
            queryStr = "update device set phone='"+str+"' where rec_id="+QString::number(rec.at(i).toInt(),10);
            tq->exec(queryStr);
        }
    }
}

void monitor::getDB()
{
    dump.clear();
    QSqlQuery *tq = new QSqlQuery();
    
    QString queryStr = "select rec_id,phone from device";
    if(!tq->exec(queryStr))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
        NeedQuit();
    }
    else
    {
        if(tq->size()>0)
        {
            if (tq->isActive())
            {
                tq->first();
                rec.append(QString::number(tq->value(0).toInt(),10));
                dump.append(tq->value(1).toString());
                while(tq->next())
                {
                    rec.append(QString::number(tq->value(0).toInt(),10));
                    dump.append(tq->value(1).toString());
                }
            }
        }
    }
    delete tq;
}

void monitor::PhoneUpdate()
{
    QSqlQuery *tq = new QSqlQuery();
    QString str;
    bool key1=false,key2=false,key3=false,key_addcol=false;;
    QString queryStr;


    //расширение таблицы device
    if(!tq->exec("select delta_5, delta_16, data_mode from device"))
    {
        tq->exec("ALTER TABLE device "
            "ADD delta_5 INTEGER DEFAULT 0, "
            "ADD delta_6 INTEGER DEFAULT 0, "
            "ADD delta_7 INTEGER DEFAULT 0, "
            "ADD delta_8 INTEGER DEFAULT 0, "
            "ADD delta_9 INTEGER DEFAULT 0, "
            "ADD delta_10 INTEGER DEFAULT 0, "
            "ADD delta_11 INTEGER DEFAULT 0, "
            "ADD delta_12 INTEGER DEFAULT 0, "
            "ADD delta_13 INTEGER DEFAULT 0, "
            "ADD delta_14 INTEGER DEFAULT 0, "
            "ADD delta_15 INTEGER DEFAULT 0, "
            "ADD delta_16 INTEGER DEFAULT 0, "
            "ADD data_mode INTEGER DEFAULT 0;");	
    }

    if(!tq->exec("select counter_number, counter_type from device"))
    {
        tq->exec("ALTER TABLE device "
            "ADD counter_number INTEGER DEFAULT 0, "
            "ADD counter_type INTEGER DEFAULT 0;");	
    }

    if(!tq->exec("select counter_1_rm_text, counter_2_rm_text from device"))
    {
        tq->exec("ALTER TABLE device "
            "ADD counter_1_rm_text VARCHAR(20) DEFAULT NULL, "
            "ADD counter_2_rm_text VARCHAR(20) DEFAULT NULL, "
            "ADD counter_1_rm_low_int INTEGER DEFAULT 0, "
            "ADD counter_2_rm_low_int INTEGER DEFAULT 0, "
            "ADD counter_1_rm_hi_int INTEGER DEFAULT 0, "
            "ADD counter_2_rm_hi_int INTEGER DEFAULT 0, "
            "ADD counter_1_rm_date VARCHAR(20) DEFAULT NULL, "
            "ADD counter_2_rm_date VARCHAR(20) DEFAULT NULL;"
            );	
    }

    queryStr.clear();
    str.clear();
    str.append(tr("Запрос тарифа"));
    if(!tq->exec("update send_commands set human_descr=\""+str+"\" where raw_val=6"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }

    queryStr.clear();
    str.clear();
    str.append(tr("Запрос тарифа"));
    if(!tq->exec("update send_commands set human_descr=\""+str+"\" where raw_val=7"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }

    
    queryStr.clear();
    str.clear();
    str.append(tr("Выполнено"));
    if(!tq->exec("update send_state set human_descr=\""+str+"\" where raw_val=0"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    

    queryStr.clear();
    str.clear();
    //активность кнопок от прав
    if(!tq->exec("delete from roles_access where name='monitor_IPWindow_pushButton_17' and role=2"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (2,\"monitor_IPWindow_pushButton_17\",\"Монитор\",\"Окно ШУНО\",\"Управление прибором\",1");
        queryStr = "insert into roles_access (role,name,human_application_name,human_window_name,human_control_name,is_enabled) "+str+");";
        tq->exec(queryStr);
    }
    if(!tq->exec("delete from roles_access where name='monitor_IPWindow_pushButton_17' and role=1"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (1,\"monitor_IPWindow_pushButton_17\",\"Монитор\",\"Окно ШУНО\",\"Управление прибором\",0");
        queryStr = "insert into roles_access (role,name,human_application_name,human_window_name,human_control_name,is_enabled) "+str+");";
        tq->exec(queryStr);
    }

    ////////////////////////////////
    queryStr.clear();
    str.clear();
    if(!tq->exec("delete from roles_access where name='monitor_IPWindow_pushButton_18' and role=2"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (2,\"monitor_IPWindow_pushButton_18\",\"Монитор\",\"Окно ШУНО\",\"Составной запрос\",1");
        queryStr = "insert into roles_access (role,name,human_application_name,human_window_name,human_control_name,is_enabled) "+str+");";
        tq->exec(queryStr);
    }
    queryStr.clear();
    str.clear();
    if(!tq->exec("delete from roles_access where name='monitor_IPWindow_pushButton_18' and role=1"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (1,\"monitor_IPWindow_pushButton_18\",\"Монитор\",\"Окно ШУНО\",\"Составной запрос\",0");
        queryStr = "insert into roles_access (role,name,human_application_name,human_window_name,human_control_name,is_enabled) "+str+");";
        tq->exec(queryStr);
    }

    //////////////////////////////////////////////////////////////////////
    queryStr.clear();
    str.clear();
    if(!tq->exec("delete from roles_access where name='monitor_IPWindow_pushButton_19' and role=2"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        //QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (2,\"monitor_IPWindow_pushButton_19\",\"Монитор\",\"Окно ШУНО\",\"Установка параметров\",1");
        queryStr = "insert into roles_access (role,name,human_application_name,human_window_name,human_control_name,is_enabled) "+str+");";
        tq->exec(queryStr);
    }

    queryStr.clear();
    str.clear();
    if(!tq->exec("delete from roles_access where name='monitor_IPWindow_pushButton_19' and role=1"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (1,\"monitor_IPWindow_pushButton_19\",\"Монитор\",\"Окно ШУНО\",\"Установка параметров\",0");
        queryStr = "insert into roles_access (role,name,human_application_name,human_window_name,human_control_name,is_enabled) "+str+");";
        tq->exec(queryStr);
    }

    str=tr("КН - Вкл, КВ - Выкл");
    queryStr = "update send_commands set human_descr='"+str+"' where raw_val=9";
    tq->exec(queryStr);
    str=tr("КН - Выкл, КВ - Вкл");
    queryStr = "update send_commands set human_descr='"+str+"' where raw_val=10";
    tq->exec(queryStr);

    str=tr("Запрос смещений ПАР");
    queryStr = "update send_commands set human_descr='"+str+"' where raw_val=14";
    tq->exec(queryStr);
    str=tr("Запрос конфигурации");
    queryStr = "update send_commands set human_descr='"+str+"' where raw_val=21";
    tq->exec(queryStr);
    str=tr("Передать конфигурацию");
    queryStr = "update send_commands set human_descr='"+str+"' where raw_val=17";
    tq->exec(queryStr);

    if(!tq->exec("delete from send_commands where raw_val=24"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (24,\"Установка параметров\"");
        queryStr = "insert into send_commands (raw_val,human_descr) "+str+");";
        tq->exec(queryStr);
    }
    if(!tq->exec("delete from send_commands where raw_val=25"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (25,\"Cоставной запрос\"");
        queryStr = "insert into send_commands (raw_val,human_descr) "+str+");";
        tq->exec(queryStr);
    }
    if(!tq->exec("delete from send_commands where raw_val=26"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"), err.text().toUtf8().constData(),
        // old QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        str=tr("values (26,\"Управление прибором\"");
        queryStr = "insert into send_commands (raw_val,human_descr) "+str+");";
        tq->exec(queryStr);
    }

    /*if(!tq->exec("select new_name from device "))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }*/


    /*if(!tq->exec("delete from roles_access where role=1 and name='monitor_IPWindow_pushButton_16'"))
    {
        QSqlError err = tq->lastError();
        QMessageBox::critical(0, qApp->tr("Ошибка"),err.text().toAscii().constData(),
                    QMessageBox::Ok,QMessageBox::NoButton);
    }*/


    /*if(tq->exec("select raw_val from send_commands"))
    {
        if (tq->isActive())
        {
            tq->first();
            while(tq->next())
            {
                if(tq->value(0).toInt()==24)
                    key1=true;
                if(tq->value(0).toInt()==25)
                    key2=true;
                if(tq->value(0).toInt()==26)
                    key3=true;
            }
        }
    }

    //if(!key1)
    //{
        str=tr("values (24,\"Установка параметров\"");
        queryStr = "insert into send_commands (raw_val,human_descr) "+str+");";
        tq->exec(queryStr);
    //}

    //if(!key2)
    //{
        str=tr("values (25,\"Составной запрос\"");
        queryStr = "insert into send_commands (raw_val,human_descr) "+str+");";
        tq->exec(queryStr);
    //}
    //if(!key3)
    //{
        str=tr("values (26,\"Управление прибором\"");
        queryStr = "insert into send_commands (raw_val,human_descr) "+str+");";
        tq->exec(queryStr);
    //}
    }*/

    ///////////////////////////////////////////////////////////////////////////////////

    /*srand(time(NULL));
    int pos=-1,max_len,tp=-1;
    QSqlQuery *tq_device = new QSqlQuery();

    str.clear();
    
    tq_device->exec("select rec_id, name from device order by rec_id asc");
    tq->exec("select count(rec_id) from device");
    tq->next();
    max_len=tq->value(0).toInt();
    for(int i=0;i<max_len;i++)
    {
        str.append("2786");
        pos=1001+rand()%1000;
        tp=rand()%7+1;
        str.append(QString::number(pos,10));
        tq_device->next();
        tq->exec("update device set counter_number="+str+" where rec_id="+QString::number(tq_device->value(0).toInt(),10));
        tq->exec("update device set counter_type="+QString::number(tp,10)+" where rec_id="+QString::number(tq_device->value(0).toInt(),10));
        str.clear();
    }*/
    //////////////////////////////////////////////////////////////////////////////////
    delete tq;

    if(startValidater())
    {
        replacePhone();
    }
}

void monitor::GetCPUID()
{
    __cpuid(CPUInfo, 1);
}

bool monitor::Security()
{
    bool key=false;

    __cpuid(CPUInfo, 1);

    QSettings *settings = new QSettings("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);
    int id=settings->value("SOFTWARE/Microsoft/ODISW/Legality").toInt();

    if((id-27)==CPUInfo[0])
        key=true;

    delete settings;
    return key;
}

void monitor::show_help()
{
    if(help_process)
    {
        QStringList arguments;
        QString path=QCoreApplication::applicationDirPath();
        path+="/help_monitor.chm";
        arguments << path;
        help_process->startDetached("hh.exe",arguments);
    }
}

void monitor::SortDevices(DevList & deviceListToSort,MonitorShowMode show_mode)
{
    BaseSettings *bs = BaseSettings::createInstance();
    int sorting_mode=0;
    switch (show_mode)
    {
    case MODE_ALL:	sorting_mode=bs->settings->value("sort/all",sorting_mode).toInt();
                    if (!sorting_mode) 
                    {
                        sorting_mode=position;
                        bs->settings->setValue("sort/all",sorting_mode);
                    }
        break;
    case MODE_GG:	sorting_mode=bs->settings->value("sort/geoarea",sorting_mode).toInt();
                    if (!sorting_mode) 
                    {
                        sorting_mode=position;
                        bs->settings->setValue("sort/geoarea",sorting_mode);
                    }
        break;
    case MODE_GA:	sorting_mode=bs->settings->value("sort/usergroups",sorting_mode).toInt();
                    if (!sorting_mode) 
                    {
                        sorting_mode=position;
                        bs->settings->setValue("sort/usergroups",sorting_mode);
                    }
        break;
    case MODE_STATE:sorting_mode=bs->settings->value("sort/devstate",sorting_mode).toInt();
                    if (!sorting_mode) 
                    {
                        sorting_mode=position;
                        bs->settings->setValue("sort/devstate",sorting_mode);
                    }
        break;
    case MODE_COUNT:sorting_mode=bs->settings->value("sort/countergroups",sorting_mode).toInt();
                    if (!sorting_mode) 
                    {
                        sorting_mode=position;
                        bs->settings->setValue("sort/countergroups",sorting_mode);
                    }
        break;
    default:		sorting_mode=bs->settings->value("sort/all",sorting_mode).toInt();
                    if (!sorting_mode) 
                    {
                        sorting_mode=position;
                        bs->settings->setValue("sort/all",sorting_mode);
                    }
        break;
    }
    //!!!SEVAST!!!
    //sorting_mode=position;
    switch (sorting_mode)
    {
    case name_num:	qSort(deviceListToSort.begin(),deviceListToSort.end(),SortDevicesNameNum);//нормальный формат, первое вхождение числа
        break;
    case name_num_r:qSort(deviceListToSort.begin(),deviceListToSort.end(),RSortDevicesNameNum);//нормальный формат, первое вхождение числа
        break;
    case position:
    default:		qSort(deviceListToSort.begin(),deviceListToSort.end(),SortDevicesPosition);//старый формат
        break;
    /*case name_str_r:qSort(deviceListToSort.begin(),deviceListToSort.end(),RSortDevicesNameStr);//старый формат
        break;
    case phone:		qSort(deviceListToSort.begin(),deviceListToSort.end(),SortDevicesPhone);
        break;
    case phone_r:	qSort(deviceListToSort.begin(),deviceListToSort.end(),RSortDevicesPhone);
        break;*/
    }
}

bool SortDevicesNameNum(const IPObject *shuno1, const IPObject *shuno2)//нормальный формат, первое вхождение числа
{
     QRegExp rx("\\d+");
     int pos = rx.indexIn(shuno1->deviceDescr.name.toLower());
     int name1 = rx.capturedTexts().first().toInt();
     pos = rx.indexIn(shuno2->deviceDescr.name.toLower());
     int name2 = rx.capturedTexts().first().toInt();
     return name1 < name2;
}

bool RSortDevicesNameNum(const IPObject *shuno1, const IPObject *shuno2)//нормальный формат, первое вхождение числа
{
     QRegExp rx("\\d+");
     int pos = rx.indexIn(shuno1->deviceDescr.name.toLower());
     int name1 = rx.capturedTexts().first().toInt();
     pos = rx.indexIn(shuno2->deviceDescr.name.toLower());
     int name2 = rx.capturedTexts().first().toInt();
     return name1 > name2;
}

bool SortDevicesPosition(const IPObject *shuno1, const IPObject *shuno2)//нормальный формат, первое вхождение числа
{
    return shuno1->deviceDescr.position < shuno2->deviceDescr.position;
}

bool SortDevicesNameStr(const IPObject *shuno1, const IPObject *shuno2)
{
     return shuno1->deviceDescr.name.toLower() < shuno2->deviceDescr.name.toLower();
}

bool RSortDevicesNameStr(const IPObject *shuno1, const IPObject *shuno2)
{
     return shuno1->deviceDescr.name.toLower() > shuno2->deviceDescr.name.toLower();
}

bool SortDevicesPhone(const IPObject *shuno1, const IPObject *shuno2)
{
    return shuno1->deviceDescr.phone.toLower() < shuno2->deviceDescr.phone.toLower();
}

bool RSortDevicesPhone(const IPObject *shuno1, const IPObject *shuno2)
{
    return shuno1->deviceDescr.phone.toLower() > shuno2->deviceDescr.phone.toLower();
}

void monitor::on_cb_SortingMode_currentIndexChanged(int sorting_mode)
{
    BaseSettings *bs = BaseSettings::createInstance();
    //сохранить в ini новое значение сортировки для текущей выборки,
    switch (currMode)
    {
    case MODE_ALL:	bs->settings->setValue("sort/all",sorting_mode);
        break;
    case MODE_GG:	bs->settings->setValue("sort/geoarea",sorting_mode);
        break;
    case MODE_GA:	bs->settings->setValue("sort/usergroups",sorting_mode);
        break;
    case MODE_STATE:bs->settings->setValue("sort/devstate",sorting_mode);
        break;
    case MODE_COUNT:bs->settings->setValue("sort/countergroups",sorting_mode);
        break;
    default:		bs->settings->setValue("sort/all",sorting_mode);
        break;
    }

    //перечитать группы с новой сортировкой.
    updateAfterConfig();
}

int monitor::getTimeDifference()//разница между (разницей между локальным временем диспетчера и UTC диспетчера) и (разницей между ПАР прибора и UTC прибора) в минутах
{
    BaseSettings *bs = BaseSettings::createInstance();
    QDateTime localTime = QDateTime::currentDateTime();
    fprintf(stdout,"\nLocal Time:\t");
    fprintf(stdout, localTime.toString("yyyy.MM.dd HH:mm:ss").toUtf8().constData());
    // old fprintf(stdout,localTime.toString("yyyy.MM.dd HH:mm:ss").toAscii().constData());

    //fprintf(stdout,"");
    fprintf(stdout,"\t\t[%i]",localTime.toTime_t());//linux time
    
    QDateTime utcTime = localTime.toUTC();
    utcTime.setTimeSpec(Qt::LocalTime);
    fprintf(stdout,"\nUTC Time:\t");
    fprintf(stdout, utcTime.toString("yyyy.MM.dd HH:mm:ss").toUtf8().constData());
    // old fprintf(stdout,utcTime.toString("yyyy.MM.dd HH:mm:ss").toAscii().constData());

    fprintf(stdout,"\t\t[%i]",utcTime.toTime_t());

    float localUTCDiff = (localTime.toTime_t() - utcTime.toTime_t())/60;//разница между локальным временем диспетчера и UTC диспетчера в минутах 
    fprintf(stdout,"\nLocal & UTC Difference: \t%f minute(s)",localUTCDiff);
    float terminalUTCDiff = bs->settings->value("System/terminal_utc_difference",2).toFloat()*60;
    fprintf(stdout,"\nTerminal UTC Difference:\t%f minute(s)",terminalUTCDiff);

    fprintf(stdout,"\n\n");
    //bs->freeInst();
    return int(localUTCDiff-terminalUTCDiff);//возвращаем в минутах
}

void monitor::get_Device_Click(IPObject * master)
{
    informationWindow->iShow(master);
}

void monitor::LoadAvtonom()//IPObject* master
{
    //убрать прибавление дельты, переделать преобразование времени ПАР по UTC+часовой пояс терминала, дельту прибавлять при отображении
    avto.tm[0].clear();
    avto.tm[1].clear();
    avto.tm[2].clear();
    avto.tm[3].clear();

    avto.state[0]=0;
    avto.state[1]=0;
    avto.state[2]=0;
    avto.state[3]=0;
    
    BaseSettings *bs = BaseSettings::createInstance();
    QDir dir;
    QString filePath=dir.absoluteFilePath(bs->settings->value("NamePARFile/name").toString());
    filePath.append(".bin");
    file=new QFile(filePath);
    
    if (file->open(QIODevice::ReadOnly)) 
    {
        QString str;
        QDataStream in(file); 
        in.setVersion(5); 

        int m,d,s; 
        for(m=0;m<12;m++)
        {
            for(d=0;d<31;d++)
            {
                for(s=0;s<4;s++)
                {       
                    Event_Table.Event[m][d][s].M=0;
                    Event_Table.Event[m][d][s].H=0;
                    Event_Table.Event[m][d][s].Event=0;
                    Event_Table1.Event[m][d][s].M=0;
                    Event_Table1.Event[m][d][s].H=0;
                }
            }
        }
        for(m=0;m<12;m++)
        {
            for(d=0;d<31;d++)
            {
                for(s=0;s<4;s++)
                {       
                    in>>Event_Table.Event[m][d][s].Event>>Event_Table.Event[m][d][s].H>>Event_Table.Event[m][d][s].M;
                }
            }
        }
    }
    QDateTime currentTime = QDateTime::currentDateTime();
    QDate date_cur=currentTime.date();
    
    
    int hour,min;
    for (int i=0;i<4;i++)
    {
        if(Event_Table.Event[date_cur.month()-1][date_cur.day()-1][i].Event==0)	avto.key[i]=false;
        else
        {
            avto.key[i]=true;
            avto.state[i]=Event_Table.Event[date_cur.month()-1][date_cur.day()-1][i].Event&0x0F;
            hour=Event_Table.Event[date_cur.month()-1][date_cur.day()-1][i].H;
            min=Event_Table.Event[date_cur.month()-1][date_cur.day()-1][i].M;
            if(hour<10 && min<10)			avto.tm[i]=avto.tm[i]+"0"+QString::number(hour)+":0"+QString::number(min);
            else if(hour<10 && min>=10)		avto.tm[i]=avto.tm[i]+"0"+QString::number(hour)+":"+QString::number(min);
            else if(hour>=10 && min<10)		avto.tm[i]=avto.tm[i]+QString::number(hour)+":0"+QString::number(min);
            else if(hour>=10 && min>=10)	avto.tm[i]=avto.tm[i]+QString::number(hour)+":"+QString::number(min);
        }
    }

    bs->freeInst();
    delete file;
}
