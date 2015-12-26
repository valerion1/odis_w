#include "ipwindow.h"
#include "logmodel.h"
#include "ipconfigurator.h"
#include "ipobject.h"
#include "basesettings.h"
#include "additionalbuttons.h"
#include "notesipw.h"
#include "monitor.h"
#include <math.h>
#include <QSize>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QSqlRecord>
#include <QTextStream>
#include <QFile>
#include <QKeyEvent>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QPicture>
#include <QPixmap>
#include <QBitmap>
#include <QDir>
#include <QDateTime>
#include <QtWidgets/QTableWidgetItem>
#include <QTimer>
#include <QAbstractSocket>


IPWindow::IPWindow(QWidget *parent)
    : QWidget(parent)
{
    BaseSettings *bs = BaseSettings::createInstance();

    ui.setupUi(this);
    split_par=bs->settings->value("IPWindow/split_par",false).toBool();
    buttons_config=bs->settings->value("Options/buttons_config","0").toInt();
    key_login=false;
    key_solut=false;
    key_count=false;
    key_anycommand=false;
    key_count=false;

    anim=new QTimer();
    connect(anim,SIGNAL(timeout()),this,SLOT(onanimTimer()));

    model = new LogModel(this);

    //mainBs = BaseSettings::createInstance();
    k1 = 0;
    k2 = 0;
    ui.pbK1->setCheckable(true);
    ui.pbK2->setCheckable(true);
    ui.pbK1->setChecked(false);
    ui.pbK2->setChecked(false);
    ui.pbK1->setGeometry(0,0,36,36);
    ui.pbK2->setGeometry(0,0,36,36);
    ui.pbK1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui.pbK2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui.pbK1->setIcon(QIcon("./Resources/k1Off.png"));
    ui.pbK2->setIcon(QIcon("./Resources/k2Off.png"));
    //ui.pbK1->setIcon(QIcon(":/monitor/Resources/k1Off.png"));
    //ui.pbK2->setIcon(QIcon(":/monitor/Resources/k2Off.png"));
    //this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::Window|Qt::WindowSystemMenuHint|Qt::WindowTitleHint);
    //this->setFixedSize(950,600);

    history=new lavelhistory(NULL);
    notedlg=NULL;
    key_anim=false;
    count_anim=0;
    key_hisgsm=false;
    parent=NULL;

    //tm_state= new QTimer();
    //connect(tm_state,SIGNAL(timeout()),this,SLOT(tm_stateTimer()));
    //num_state=-1;
    //state_inter=150;

    //tm_load=new QTimer();
    //connect(tm_load,SIGNAL(timeout()),this,SLOT(tm_loadTimer()));

    pbMskRequest=NULL;
    pbDeltaRequest=NULL;
    pbMskRequest = new QPushButton(ui.groupBox);
    pbDeltaRequest = new QPushButton(ui.groupBox);
    //pbSetData = new QPushButton(ui.groupBox);
    pbMskRequest->setObjectName(QString::fromUtf8("pushButton_MskRequest"));
    pbDeltaRequest->setObjectName(QString::fromUtf8("pushButton_DeltaRequest"));
    //pbSetData->setObjectName(QString::fromUtf8("pushButton_19"));
    pbMskRequest->setText(tr("Конфигурация"));
    pbDeltaRequest->setText(tr("Смещение ПАР"));
    //pbSetData->setText(tr("Установка параметров"));
    ui.ipRequestLayout->addWidget(pbMskRequest);
    //ui.vboxLayout5->addWidget(pbSetData);
    ui.ipRequestLayout->addWidget(pbDeltaRequest);

    //connect(pbSetData,SIGNAL(clicked()),this,SLOT(on_pushButton_17_clicked()));
    connect(pbDeltaRequest,SIGNAL(clicked()),this,SLOT(pushButton_DeltaRequest_clicked()));
    connect(pbMskRequest,SIGNAL(clicked()),this,SLOT(pushButton_MskRequest_clicked()));

    //connect(mainBs->ipConfiguratorWindow,SIGNAL(ipconf_signalAllToServer()),this,SLOT(ipconf_signalAllToServer()));
    //connect(mainBs->ipConfiguratorWindow,SIGNAL(ipconf_putCommand(int, int, int)),this,SLOT(ipconf_putCommand(int, int, int)));

    //getdlg=NULL;
    //senddlg=NULL;
    //setdlg=NULL;
    //getdlg=new GetDataDialog(NULL);
    //senddlg=new CommandDialog(NULL);
    //setdlg=new SendDataDialog(NULL);

    //getdlg->setParent(this);
    //senddlg->setParent(this);
    //setdlg->setParent(this);
    
    //connect(senddlg,SIGNAL(ToServer(int)),this,SLOT(ToServer(int)));
    //connect(getdlg,SIGNAL(ToServer(int)),this,SLOT(ToServer(int)));
    //connect(setdlg,SIGNAL(ToServer(int)),this,SLOT(ToServer(int)));

    moredlg=NULL;//дополнительное окно для показаний счётчика
    moredlg= new more_counter();
    moredlg->setpParent(this);
    ui.stepsframe->installEventFilter(this);
    bs->freeInst();
}

IPWindow::~IPWindow()
{
    //BaseSettings *bs = BaseSettings::createInstance();
    parent->mainBs->settings->setValue("IPWindow/last_position",saveGeometry());//сохраняет положение окна в .ini файле
    parent->mainBs->settings->setValue("IPWindow/split_par",QString::number(split_par,2));
    parent->mainBs->settings->sync();
    if (parent->ipConfiguratorWindow->isVisible()) parent->ipConfiguratorWindow->hide();//говно
    delete parent->ipConfiguratorWindow;
    if (notedlg) delete notedlg;
    if (anim->isActive()) anim->stop();
    if (anim) delete anim;
    if (history) delete history;
    //if(tm_state) delete tm_state;
    //if(tm_load) delete tm_load;
    if (pbMskRequest) delete pbMskRequest;
    if (pbDeltaRequest) delete pbDeltaRequest;
    //if(pbSetData) delete pbSetData;
    //if (getdlg) delete getdlg;
    //if (senddlg) delete senddlg;
    //if (getdlg) delete setdlg;
    if (moredlg) delete moredlg;
    //if (stepsIPW) delete stepsIPW;
    parent->mainBs->freeInst();
}

void IPWindow::setParent(monitor* ptr)
{
    parent=(monitor*)ptr;
}

void IPWindow::SignaltoMap(IPObject *in_master)
{
    parent->SignaltoMap(in_master->rec_id);
}

void IPWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) hide();
}

void IPWindow::closeEvent(QCloseEvent *event)
{
    master->key_anim=false;
    count_anim=0;
    key_anim=false;
    key_hisgsm=false;
}

void IPWindow::sortParSteps(_step *psteps)
{
    qsort(psteps,8,sizeof(_step),&IPWindow::sort_steps);//сортирует по времени, неактивные шаги сдвигает тупо вниз
}

int IPWindow::sort_steps(const void * p1,const void * p2)
{//дополнительная функция для qsort
    _step* struct1 = (_step*)p1;
    _step* struct2 = (_step*)p2;

    int diff_activity = struct2->act - struct1->act;
    if (diff_activity) return diff_activity;
    if (struct1->time.addSecs(struct1->delta*60+monitor::getTimeDifference()*60) < struct2->time.addSecs(struct2->delta*60+monitor::getTimeDifference()*60)) return -1;//сортируем по локальному времени монитора
    else 
    if (struct1->time.addSecs(struct1->delta*60+monitor::getTimeDifference()*60) == struct2->time.addSecs(struct2->delta*60+monitor::getTimeDifference()*60)) return  0;//сортируем по локальному времени монитора
    else return  1;
}

void IPWindow::refreshParSteps(_step *psteps)
{
    psteps[0].num=0;    psteps[0].act=parent->avto.key[0];              psteps[0].time=QTime::fromString(parent->avto.tm[0],"hh:mm");   psteps[0].knkv=parent->avto.state[0];   psteps[0].delta=master->deviceDescr.delta_1;                        psteps[0].primary=1;    psteps[0].changed=0;
    psteps[1].num=1;    psteps[1].act=parent->avto.key[1];              psteps[1].time=QTime::fromString(parent->avto.tm[1],"hh:mm");   psteps[1].knkv=parent->avto.state[1];   psteps[1].delta=master->deviceDescr.delta_2;                        psteps[1].primary=1;    psteps[1].changed=0;
    psteps[2].num=2;    psteps[2].act=parent->avto.key[2];              psteps[2].time=QTime::fromString(parent->avto.tm[2],"hh:mm");   psteps[2].knkv=parent->avto.state[2];   psteps[2].delta=master->deviceDescr.delta_3;                        psteps[2].primary=1;    psteps[2].changed=0;
    psteps[3].num=3;    psteps[3].act=parent->avto.key[3];              psteps[3].time=QTime::fromString(parent->avto.tm[3],"hh:mm");   psteps[3].knkv=parent->avto.state[3];   psteps[3].delta=master->deviceDescr.delta_4;                        psteps[3].primary=1;    psteps[3].changed=0;
    psteps[4].num=12;   psteps[4].act=(delta[12].event!=0?1:0); psteps[4].time=QTime(0,0,0,0);                          psteps[4].knkv=delta[12].event; psteps[4].delta=delta[12].time.hour()*60+delta[12].time.minute();   psteps[4].primary=0;    psteps[4].changed=0;
    psteps[5].num=13;   psteps[5].act=(delta[13].event!=0?1:0); psteps[5].time=QTime(0,0,0,0);                          psteps[5].knkv=delta[13].event; psteps[5].delta=delta[13].time.hour()*60+delta[13].time.minute();   psteps[5].primary=0;    psteps[5].changed=0;
    psteps[6].num=14;   psteps[6].act=(delta[14].event!=0?1:0); psteps[6].time=QTime(0,0,0,0);                          psteps[6].knkv=delta[14].event; psteps[6].delta=delta[14].time.hour()*60+delta[14].time.minute();   psteps[6].primary=0;    psteps[6].changed=0;
    psteps[7].num=15;   psteps[7].act=(delta[15].event!=0?1:0); psteps[7].time=QTime(0,0,0,0);                          psteps[7].knkv=delta[15].event; psteps[7].delta=delta[15].time.hour()*60+delta[15].time.minute();   psteps[7].primary=0;    psteps[7].changed=0;
    sortParSteps(psteps);
}

void IPWindow::iShow(IPObject *in_master)
{
    BaseSettings *mainBs = BaseSettings::createInstance();

    //updateGeometry();
    restoreGeometry(mainBs->settings->value("IPWindow/last_position").toByteArray());//положение окна считывается из .ini файла

    model->clear();
    master = in_master;
    dbIndex = master->rec_id;
    image = *(master->details);

    this->setWindowTitle(master->deviceDescr.name);

    if(master->deviceDescr.data_mode)
    {
        //ui.pushButton_6->setText(tr("Качество связи"));
        //ui.pushButton_7->setText(tr("Показания счётчика"));
        //ui.pushButton_6->hide();
        pbMskRequest->show();
        pbMskRequest->setDisabled(FALSE);
        //pbMskRequest->setText(tr("Запросить маску"));
        pbDeltaRequest->show();
        pbDeltaRequest->setDisabled(FALSE);
        //pbDeltaRequest->setText(tr("Запросить дельты"));
    }
    else
    {
        //ui.pushButton_6->setText(tr("Тариф 1"));
        //ui.pushButton_7->setText(tr("Тариф 2"));
        //ui.pushButton_6->show();
        pbMskRequest->hide();
        pbMskRequest->setDisabled(true);
        pbDeltaRequest->hide();
        pbDeltaRequest->setDisabled(true);
    }
    //ui.pushButton_11->hide();
    //pbSetData->hide();

    //ui.pushButton_12->setText(tr("Перейти в ПАР"));
    //ui.label_6->setText(tr("События ПАР"));
    
    //ui.label_2->setPixmap(QPixmap::fromImage(face));
    ui.labelPic->setPixmap(QPixmap::fromImage(*master->details));
    ui.labelPic->installEventFilter(this);
    ui.labelPic->setMouseTracking(true);
    /*if (master->deviceDescr.is_avto)
    {
        ui.labelMode->setText(tr("Авто"));
    }
    else
    {
        ui.labelMode->setText(tr("Ручной"));
    }*/
    ui.le_phone->setText(master->deviceDescr.phone);
    ui.le_hard->setText(QString::number(master->deviceDescr.dev_hard_number,10));
    ui.le_addr->setText(master->deviceDescr.adress);
    ui.le_addr->setToolTip(master->deviceDescr.notes);
    n_name.clear();
    notes.clear();
    n_name=master->deviceDescr.name;
    notes=master->deviceDescr.notes;
    if(!notes.isEmpty())
    {
        ui.pb_notes->setEnabled(true);
        if(notedlg)
        {
            delete notedlg;
            notedlg=NULL;
        }
        notedlg=new notesIPW(NULL);
    }
    else
        ui.pb_notes->setDisabled(true);//заметки

    //QSqlQuery *tq = new QSqlQuery("update device set is_viewed=1 where rec_id="+QString::number(dbIndex,10));
    //tq->exec();
    /*if (!master->deviceDescr.is_configured)
    {
        ui.pbSendConfig->setPalette(QPalette(QColor(255,100,100)));
    }else{
        ui.pbSendConfig->setPalette(QPalette(QPalette().color(QPalette::Button)));
    }*/
    //delta
    /*ui.le_delta_1->setText(QString::number(master->deviceDescr.delta_1,10));
    ui.le_delta_2->setText(QString::number(master->deviceDescr.delta_2,10));
    ui.le_delta_3->setText(QString::number(master->deviceDescr.delta_3,10));
    ui.le_delta_4->setText(QString::number(master->deviceDescr.delta_4,10));*/

    /*if(!key_count)
    {
        ui.pushButton_6->setDisabled(true);
        ui.pushButton_7->setDisabled(true);
    }
    else
    {
        ui.pushButton_6->setDisabled(false);
        ui.pushButton_7->setDisabled(false);
    }*/
    //ui.pushButton_6->setDisabled(true);

    drawSHUNO(dbIndex);
    //this->move(this->parent->geometry().center() - this->geometry().center());
    //this->move(this->x(),availableGeometry().height());
    adjustSize();
    show();

    ui.frame_3->setMouseTracking(true);
    ui.frame_3->installEventFilter(this);
    ui.frame_3->setToolTip(tr("Нажать левую кнопку мыши, чтобы посмотреть дополнительные параметры"));

    /*QSqlQuery tq;
    tq.exec("update device set is_viewed=1 where rec_id="+QString::number(dbIndex,10));
    master->deviceDescr.is_viewed = TRUE;*/
    master->updateMainIcon();
    //delete tq;

    //tm_load->start(300);

    /*if(master->deviceDescr.k1_present)
        ui.pbK1->setEnabled(true);
    else
        ui.pbK1->setDisabled(true);
    if(master->deviceDescr.k2_present)
        ui.pbK2->setEnabled(true);
    else
        ui.pbK2->setDisabled(true);*/


    //emit putMap(master->rec_id);

    //ui.pushButton_6->setEnabled(false);
    if(key_config)
        ui.pushButton_ipConf->setEnabled(true);
    else
        ui.pushButton_ipConf->setEnabled(false);
    mainBs->freeInst();
}

void IPWindow::RedrawAutonomSteps(void)
{
    //QTime tmp_time;
    int _grid_x=0,_grid_2_x=3,_grid_y=0,_grid_2_y=0;
    QPixmap pixmap;
    QLayoutItem * temp_item;
    while ((temp_item= ui.stepsGridLayout->takeAt(0)) != 0)
    {
        delete temp_item->widget();
        delete temp_item;
    }
    //ui.stepsGridLayout->setHorizontalSpacing(5);
    //ui.stepsGridLayout->setVerticalSpacing(15);
    //ui.stepsGridLayout->set
    ////////ui.stepsGridLayout->setColumnStretch(2,1);//контакторы занимают всё лишнее пространство
    ////////if (split_par)  ui.stepsGridLayout->setColumnStretch(5,1);//контакторы занимают всё лишнее пространство
    ////////ui.stepsGridLayout->setColumnMinimumWidth(0,20);//часы узкие
    //ui.stepsGridLayout->setColumnStretch(0,1);//
    ////////if (split_par)  ui.stepsGridLayout->setColumnMinimumWidth(3,20);//часы узкие

    //ui.stepsGridLayout->colu
        //setHorizontalSpacing(5);
    //ui.stepsGridLayout->setVerticalSpacing(5);
    //QTimeEdit step_time[8];
    for (int i=0;i<9;i++)
    {
        //ui.stepsGridLayout->setRowMinimumHeight(i,35);
        //ui.stepsGridLayout->setRowStretch(i,1);
        //tmp_time=QTime(0,0,0,0);
        _grid_x=0;
        _grid_2_x=3;
        if (i==0)
        {//заголовок
            QLabel *time_header=new QLabel(tr("время шага"),this);
            time_header->setStyleSheet("color:white;background-color:#777;");
            time_header->setAlignment(Qt::AlignCenter);
            time_header->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            time_header->setContentsMargins(2,1,2,1);
            ui.stepsGridLayout->addWidget(time_header,_grid_y,_grid_x,1,2);
            if (split_par)//в две колонки
            {
                QLabel *time_2_header=new QLabel(tr("время доп. шага"),this);
                time_2_header->setAlignment(Qt::AlignCenter);
                time_2_header->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
                time_2_header->setContentsMargins(2,1,2,1);
                time_2_header->setStyleSheet("color:white;background-color:#777;");
                ui.stepsGridLayout->addWidget(time_2_header,_grid_2_y,_grid_2_x,1,2);
            }
            _grid_x++;
            _grid_2_x++;
            //QLabel *delta_header=new QLabel(tr("сдвиг"),this);
            //ui.stepsGridLayout->addWidget(delta_header,_grid_y,_grid_x);
            _grid_x++;
            _grid_2_x++;
            QLabel *event_header=new QLabel(tr(" КН   КВ"),this);
            event_header->setAlignment(Qt::AlignLeft);
            event_header->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
            event_header->setStyleSheet("color:white;background-color:#777;");
            ui.stepsGridLayout->addWidget(event_header,_grid_y,_grid_x);
            if (split_par) 
            {
                QLabel *event_2_header=new QLabel(tr(" КН   КВ"),this);
                event_2_header->setAlignment(Qt::AlignLeft);
                event_2_header->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
                event_2_header->setStyleSheet("color:white;background-color:#777;");
                //event_2_header->setContentsMargins(5,1,5,1);
                ui.stepsGridLayout->addWidget(event_2_header,_grid_2_y,_grid_2_x);
            }
            _grid_y++;
            _grid_2_y++;
        }//конец заголовка
        else
        {
            //ui.stepsGridLayout->setRowMinimumHeight(i,16);//не обрезает картинки
            step_delta[i-1]=new QLabel(this);
            step_delta[i-1]->setAlignment(Qt::AlignCenter);
            step_delta[i-1]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            step_delta[i-1]->setStyleSheet("color:black;background-color:#bbb;");
            step_delta[i-1]->setMinimumWidth(18);
            step_time[i-1]=new QLabel(this);
            step_time[i-1]->setAlignment(Qt::AlignCenter);
            step_time[i-1]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
            step_time[i-1]->setStyleSheet("color:black;background-color:#bbb;");
            step_event[i-1]=new QLabel(this);
            step_event[i-1]->setAlignment(Qt::AlignLeft);
            step_event[i-1]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
            step_event[i-1]->setStyleSheet("color:black;background-color:#bbb;");

            if (stepsIPW[i-1].act)
            {//если шаг активен, рисуем чё-нить полезное
                if (stepsIPW[i-1].primary)
                {
                    if (stepsIPW[i-1].delta!=0)//на основном шаге присутствует дельта, обозначим жёлтыми часиками
                    {
                        pixmap.load("./Resources/deltaPRI.png");
                        pixmap=pixmap.scaledToHeight(16);
                        step_delta[i-1]->setPixmap(pixmap);
                        step_delta[i-1]->setToolTip(tr("основной шаг ")+stepsIPW[i-1].time.toString("hh:mm")+tr(" по часовому поясу терминала, смещение ")+QString::number(stepsIPW[i-1].delta)+tr(" минут."));
                        step_time[i-1]->setToolTip(tr("основной шаг ")+stepsIPW[i-1].time.toString("hh:mm")+tr(" по часовому поясу терминала, смещение ")+QString::number(stepsIPW[i-1].delta)+tr(" минут."));
                    }
                    else 
                    {
                        step_delta[i-1]->setToolTip(tr("основной шаг ")+stepsIPW[i-1].time.toString("hh:mm")+tr(" по часовому поясу терминала, без смещения."));
                        step_time[i-1]->setToolTip(tr("основной шаг ")+stepsIPW[i-1].time.toString("hh:mm")+tr(" по часовому поясу терминала, без смещения."));
                    }
                    qDebug()<<"pixmap delta size:"<<pixmap.size().width()<<"x"<<pixmap.size().height()<<"\n";
                    qDebug()<<"delta size:"<<step_delta[i-1]->size().width()<<"x"<<step_delta[i-1]->size().height()<<"\n";
                }//primary
                else 
                {//присутствует дополнительный шаг, обозначим синими часиками
                    pixmap.load("./Resources/deltaADD.png");
                    //pixmap=pixmap.scaledToWidth(16);
                    pixmap=pixmap.scaledToHeight(16);
                    step_delta[i-1]->setPixmap(pixmap);
                    step_delta[i-1]->setToolTip(tr("дополнительный шаг ")+stepsIPW[i-1].time.addSecs(stepsIPW[i-1].delta*60).toString("hh:mm")+tr(" по часовому поясу терминала."));
                    step_time[i-1]->setToolTip(tr("дополнительный шаг ")+stepsIPW[i-1].time.addSecs(stepsIPW[i-1].delta*60).toString("hh:mm")+tr(" по часовому поясу терминала."));
                }
                step_time[i-1]->setText(stepsIPW[i-1].time.addSecs(stepsIPW[i-1].delta*60+monitor::getTimeDifference()*60).toString("hh:mm"));//время терминала >> локальное в мониторе//stepsIPW[i].delta*60

                switch(stepsIPW[i-1].knkv)
                {
                    case 10:pixmap.load("./Resources/auto_0_0.png");
                            step_event[i-1]->setToolTip(tr("выключение"));
                            break;
                    case 11:pixmap.load("./Resources/auto_1_0.png");
                            step_event[i-1]->setToolTip(tr("частичное выключение"));
                            break;
                    case 14:pixmap.load("./Resources/auto_0_1.png");
                            step_event[i-1]->setToolTip(tr("частичное выключение"));
                            break;
                    case 15:pixmap.load("./Resources/auto_1_1.png");
                            step_event[i-1]->setToolTip(tr("включение"));
                            break;
                }
                //pixmap=pixmap.scaledToWidth(58);
                pixmap=pixmap.scaledToHeight(16);
                step_event[i-1]->setPixmap(pixmap);//рисуем лампочки
                qDebug()<<"pixmap event size:"<<pixmap.size().width()<<"x"<<pixmap.size().height()<<"\n";
                qDebug()<<"event size:"<<step_event[i-1]->size().width()<<"x"<<step_event[i-1]->size().height()<<"\n";
                //step_delta[i-1]->setContentsMargins(5,1,5,1);
                //step_time[i-1]->setContentsMargins(3,3,3,3);
                //step_event[i-1]->setContentsMargins(5,1,5,1);
            }//шаг активный
        //else//нарисуем пустое поле в случае split_par==1
            if (split_par)//две колонки
            {
                if (stepsIPW[i-1].primary)//основной шаг слева
                {
                    ui.stepsGridLayout->addWidget(step_delta[i-1],_grid_y,_grid_x);
                    _grid_x++;
                    ui.stepsGridLayout->addWidget(step_time[i-1],_grid_y,_grid_x);
                    _grid_x++;
                    ui.stepsGridLayout->addWidget(step_event[i-1],_grid_y,_grid_x);
                    _grid_y++;
                }
                else//дополнительный справа
                {
                    ui.stepsGridLayout->addWidget(step_delta[i-1],_grid_2_y,_grid_2_x);
                    _grid_2_x++;
                    ui.stepsGridLayout->addWidget(step_time[i-1],_grid_2_y,_grid_2_x);
                    _grid_2_x++;
                    ui.stepsGridLayout->addWidget(step_event[i-1],_grid_2_y,_grid_2_x);
                    _grid_2_y++;
                }
            }
            else//одна широкая колонка
            {
                if (stepsIPW[i-1].act)//при одной колонке не нужно рисовать пустое поле для неактивного шага
                {
                    //step_event[i-1]->setText(step_event[i-1]->toolTip());//рисуем лампочки
                    ui.stepsGridLayout->addWidget(step_delta[i-1],_grid_y,_grid_x);
                    _grid_x++;
                    ui.stepsGridLayout->addWidget(step_time[i-1],_grid_y,_grid_x);
                    _grid_x++;
                    ui.stepsGridLayout->addWidget(step_event[i-1],_grid_y,_grid_x);
                    _grid_y++;
                }
            }
        }//значения под заголовком
    }
    qDebug()<<"columns: "<<ui.stepsGridLayout->columnCount()<<"\n";
}

void IPWindow::onanimTimer()
{
    if(count_anim%2==0)
        master->key_anim=true;
    else
        master->key_anim=false;
    
    if(count_anim>=13)
    {
        anim->stop();
        master->key_anim=true;
        count_anim=0;
    }
    master->updateImages();
    this->update();
    if(count_anim>=13)
        master->key_anim=false;
    count_anim++;
}

bool IPWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (obj == ui.labelPic)
  {
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent * mouseEvent = (QMouseEvent *)event; 
        if ((mouseEvent->x() > 450)&&(mouseEvent->x() < 500) && (mouseEvent->y() > 5)&&(mouseEvent->y() < 45) && key_hisgsm)
        {
            history->phone=master->deviceDescr.phone;
            history->name=master->deviceDescr.name;
            history->iShow();
        }
        else
        {
            this->setToolTip(tr(""));
            this->setCursor(Qt::ArrowCursor);
        }
        return true;
    } 
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent * mouseEvent = (QMouseEvent *)event; 
        if ((mouseEvent->x() > 450)&&(mouseEvent->x() < 500) && (mouseEvent->y() > 5)&&(mouseEvent->y() < 45) && key_hisgsm)
        {
            this->setToolTip(tr("Показать историю"));
            this->setCursor(Qt::PointingHandCursor);
        }
        else
        {
            this->setToolTip(tr(""));
            this->setCursor(Qt::ArrowCursor);
        }
        
        return true;
    }
  }
  //if (obj==ui.frame_3 && master->deviceDescr.counter_pr esent)
  if (obj==ui.frame_3 && master->deviceDescr.counter_type>-1)
  {
      if (event->type() == QEvent::MouseButtonPress)
      {
          moredlg->setName(master->deviceDescr.name);
          moredlg->iShow();
      }
      else if (event->type() == QEvent::MouseMove)
      {
          QMouseEvent * mouseEvent = (QMouseEvent *)event; 
          if ((mouseEvent->x() > ui.frame_3->rect().left())&&(mouseEvent->x() < ui.frame_3->rect().right()) && (mouseEvent->y() > ui.frame_3->rect().top())&&(mouseEvent->y() < ui.frame_3->rect().bottom()))
          {
            //this->setToolTip(tr("Показать историю"));
            this->setCursor(Qt::PointingHandCursor);
          }
          else
          {
            //this->setToolTip(tr(""));
            this->setCursor(Qt::ArrowCursor);
          }
      }

  }
    if (obj==ui.stepsframe)
    {
        if (event->type()==QEvent::MouseButtonDblClick)
        {
            split_par^=1;
            //hide();
            this->iShow(master);
            //iShow();
        }
    }
    //else
    //this->setCursor(Qt::ArrowCursor);
  
 return QWidget::eventFilter(obj, event);
}

IPObject* IPWindow::getObject(){
    return master;
}

void IPWindow::refreshFace()
{
    master->updateDevice();
    master->updateState();
    master->updateConfig();
    master->updateImages();
    //setMyConfig();
}

void IPWindow::drawSHUNO(int in_dbIndex){
    int state13,state14,state15,state16;
    master->updateImages();
//---обновление таблицы с ПАР
    parent->LoadAvtonom();
    for(int i=0;i<16;i++)
    {
        if (i<4) delta[i].primary=1;
        else delta[i].primary=0;
        delta[i].value=0;
        delta[i].time=QTime(0,0,0,0);
        delta[i].event=0;
    }
    delta[0].value=master->deviceDescr.delta_1;
    delta[1].value=master->deviceDescr.delta_2;
    delta[2].value=master->deviceDescr.delta_3;
    delta[3].value=master->deviceDescr.delta_4;
    delta[12].time=(master->deviceDescr.delta_13>=0)?delta[12].time.addSecs((master->deviceDescr.delta_13)*60):QTime(0,0,0,0);
    delta[13].time=(master->deviceDescr.delta_14>=0)?delta[13].time.addSecs((master->deviceDescr.delta_14)*60):QTime(0,0,0,0);
    delta[14].time=(master->deviceDescr.delta_15>=0)?delta[14].time.addSecs((master->deviceDescr.delta_15)*60):QTime(0,0,0,0);
    delta[15].time=(master->deviceDescr.delta_16>=0)?delta[15].time.addSecs((master->deviceDescr.delta_16)*60):QTime(0,0,0,0);
    //секция заполнения массива для сортировки шагов пара
    //в avtonom_present лежит 1 байт, от старшего бита к младшему для дополнительного автонома: 1KH 1KB 2KH 2KB 3KH 3KB 4KH 4KB
    state13=master->deviceDescr.delta_13>=0?10:0;
    state14=master->deviceDescr.delta_14>=0?10:0;
    state15=master->deviceDescr.delta_15>=0?10:0;
    state16=master->deviceDescr.delta_16>=0?10:0;
    if (state13) state13|=master->bitstate(master->deviceDescr.avtonom_present,7)<<0;//KH первого дополнительного шага
    if (state13) state13|=master->bitstate(master->deviceDescr.avtonom_present,6)<<2;//KB первого дополнительного шага
    if (state14) state14|=master->bitstate(master->deviceDescr.avtonom_present,5)<<0;//KH второго дополнительного шага
    if (state14) state14|=master->bitstate(master->deviceDescr.avtonom_present,4)<<2;//KB второго дополнительного шага
    if (state15) state15|=master->bitstate(master->deviceDescr.avtonom_present,3)<<0;//KH третьего дополнительного шага
    if (state15) state15|=master->bitstate(master->deviceDescr.avtonom_present,2)<<2;//KB третьего дополнительного шага
    if (state16) state16|=master->bitstate(master->deviceDescr.avtonom_present,1)<<0;//KH четвёртого дополнительного шага
    if (state16) state16|=master->bitstate(master->deviceDescr.avtonom_present,0)<<2;//KB четвёртого дополнительного шага
    delta[12].event=state13;
    delta[13].event=state14;
    delta[14].event=state15;
    delta[15].event=state16;
    refreshParSteps(stepsIPW);//пересчитываем, сортируем шаги
    RedrawAutonomSteps();//перерисовываем табличку с шагами ПАР
//--- вывод информации по счётчикам
    //if(master->deviceDescr.coun ter_present){
    if(master->deviceDescr.counter_type>0){//какой-нибудь счётчик есть
        ui.frame_3->setDisabled(FALSE);
        ui.pb_counter->setDisabled(FALSE);
        ui.leCounter_1->setText(master->deviceDescr.counter_1_text.left(master->deviceDescr.counter_1_text.indexOf(".")));//The entire string is returned if n is greater than size() or less than zero.
        ui.leCounter_2->setText(master->deviceDescr.counter_2_text.left(master->deviceDescr.counter_2_text.indexOf(".")));
        ui.la_date_1->setText(master->deviceDescr.counter_1_date);
        ui.la_date_2->setText(master->deviceDescr.counter_2_date);
    }
    else
    {//сётчик отсутствует
        ui.frame_3->setDisabled(TRUE);
        ui.pb_counter->setDisabled(TRUE);
        ui.leCounter_1->setText(tr("нет"));
        ui.leCounter_2->setText(tr("нет"));
        ui.la_date_1->setText(tr(" - "));
        ui.la_date_2->setText(tr(" - "));
    }
    QFont font;
    font.setPointSize(8);
    font.setBold(true);
    font.setFamily("MS Shell Dlg 2");
    font.setItalic(false);
    font.setStrikeOut(false);
    font.setUnderline(false);
    ui.label->setFont(font);
    ui.label_2->setFont(font);
    ui.label->setWordWrap(true);
    ui.label_2->setWordWrap(true);
    ui.label->setText(tr("Активная энергия\n    T1(A+) КВт/ч"));
    ui.label_2->setText(tr("Активная энергия\n    T2(A+) КВт/ч"));
    font.setPointSize(9);
    ui.labelName->setFont(font);
    font.setPointSize(15);
    font.setBold(true);
    font.setFamily("Arial");
    font.setItalic(false);
    font.setStrikeOut(false);
    font.setUnderline(false);
    ui.labelName->setFont(font);
    ui.labelName->setText(master->deviceDescr.name.toUpper());

//--- обновление лога событий
    drawHistory();

//--- обновление картинки
    ui.labelPic->setPixmap(QPixmap::fromImage(*master->details));
//--- обновление режима работы
    switch (master->deviceDescr.is_avto)
        {
        case 0:ui.labelMode->setText(tr("?"));break;
        case 1:ui.labelMode->setText(tr("Ручной"));break;
        case 2:ui.labelMode->setText(tr("Авто"));break;
        default:ui.labelMode->setText(tr("???"));break;
        }
//--- обновление состояния двери
    if(master->deviceDescr.door_present)
        {
        switch (master->deviceState.door_state)
            {
            case d_state_UNCR:
                ui.le_door->setText(tr("Неизвестно"));
                break;
            case d_state_NONE:
                ui.le_door->setText(tr("Не подключена"));
                break;
            case d_state_CLOSED:
                ui.le_door->setText(tr("Закрыта"));
                break;
            case d_state_OPENED:
                ui.le_door->setText(tr("Открыта"));
                break;
            default:
                ui.le_door->setText(tr("Неизвестно"));
                break;
                //closed
            }
        }
    else {ui.le_door->setText(tr("Не подключена"));}

    QSqlQuery tq;
    tq.exec("update device set is_viewed=1 where rec_id="+QString::number(in_dbIndex,10));
    master->deviceDescr.is_viewed = TRUE;

    this->update();
}

void IPWindow::drawHistory(){
    //нужно сделать таблицу с типами счётчика и привязать её к DATA_CNP
    QSqlQuery tq;
    // (send.command_type=send_commands.raw_val or send.command_type=send_commands.raw_val+50) 
//select 1, send.time_send, send_commands.human_descr, send_state.human_descr, 0 from send, send_commands, send_state where device_id=780 and (send.command_type=send_commands.raw_val or send.command_type=send_commands.raw_val+50) and send.command_state=send_state.raw_val 
//select 2, time_stamp, 'Состояние ШУНО', human_descr, is_error from recv_state where device_id=780 order by time_stamp resc limit 100
    QString queryStr = 
        "select 1, send.time_send, send_commands.human_descr, send_state.human_descr, 0 from send, send_commands, send_state "
        //"where device_id="+QString::number(dbIndex,10)+" and send.command_type=send_commands.raw_val and send.command_state=send_state.raw_val "
        "where device_id="+QString::number(dbIndex,10)+" and (send.command_type=send_commands.raw_val or send.command_type=send_commands.raw_val+50) and send.command_state=send_state.raw_val "
        " UNION "
        "select 2, time_stamp, "+
        tr("'Состояние ШУНО'")+
        ", human_descr, is_error from recv_state where device_id="+QString::number(dbIndex,10)+
        " UNION "
        "select 3, recv_gprs.time_stamp, "+
        tr("'Состояние GPRS'")+
        ", gprs_types.human_descr, is_error from recv_gprs, gprs_types "
        "where recv_gprs.device_id="+QString::number(dbIndex,10)+" and recv_gprs.raw_data=gprs_types.raw_val "
        " UNION "
        "select 4, time_stamp, "+
        tr("'Показания счетчика'")+
        ", human_descr, is_error from recv_counter where device_id="+QString::number(dbIndex,10)+
        " UNION "
        "select 5, time_stamp, "+
        tr("'Время ШУНО'")+
        ", human_descr, is_error from recv_time where device_id="+QString::number(dbIndex,10)+
        " UNION "
        "select 6, time_stamp, "+
        tr("'Ошибка приема'")+
        ", " +
        tr("'Ошибка приема' ")+
        ", " +
        "0 from recv_crc where device_id="+QString::number(dbIndex,10)+" order by 2 desc limit 100";

    /*tq.prepare(queryStr);
    tq.bindValue(":device_id1",dbIndex);
    tq.bindValue(":device_id2",dbIndex);
    tq.bindValue(":device_id3",dbIndex);
    tq.bindValue(":device_id4",dbIndex);
    tq.bindValue(":device_id5",dbIndex);
    tq.bindValue(":device_id6",dbIndex);*/

    
    //QMessageBox::warning(this,"Error",queryStr);
    //ItemDelegate *delegate = new ItemDelegate();

    model->setCol();
    model->clear();
    tq.exec(queryStr);
    model->setQuery(tq);
    model->setHeaderData(0, Qt::Horizontal, tr("тип"));
    model->setHeaderData(1, Qt::Horizontal, tr("дата/время"));
    model->setHeaderData(2, Qt::Horizontal, tr("команда/ответ"));
    model->setHeaderData(3, Qt::Horizontal, tr("описание"));
    //paint
//  int maxRowCount=model->rowCount();
//  bool retVal;
//  for (int i=0; i<maxRowCount;i++){
//      retVal = model->setData(model->index(i,0),QVariant(QColor(Qt::red)),Qt::BackgroundRole);
//  }
    //draw
    //ui.tableView->setMaximumWidth(530);
    ui.tableView->setModel(model);
    //ui.tableView->setItemDelegate(delegate);
    ui.tableView->setWordWrap(true);
    //ui.tableView->hideColumn(4);
    ui.tableView->verticalHeader()->setVisible(false);//нет нумерации
    ui.tableView->setColumnHidden(0,true);//нет нумерации
    ui.tableView->setColumnHidden(4,true);
    //int totalWidth = 515;
    //int comment_width=totalWidth - ui.tableView->columnWidth(1) - ui.tableView->columnWidth(2);
    //ui.tableView->setVisible(false);
    //ui.tableView->setColumnWidth(3,comment_width);//-46);
    //ui.tableView->setVisible(true);
    //ui.tableView->resizeColumnsToContents();
    //ui.tableView->resizeRowsToContents();

    ui.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //old ui.tableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui.tableView->horizontalHeader()->setStretchLastSection(true);
    ui.tableView->resizeColumnToContents(1);
    ui.tableView->resizeColumnToContents(2);
    //ui.tableView->resizeColumnToContents(3);
    //for (int i=0;i<model->rowCount();i++)
    //ui.tableView->resizeRowToContents(i);

    qDebug()<<"table width:"<<ui.tableView->viewport()->size().height()<<"x"<<ui.tableView->viewport()->size().width()<<", comment width: "<<ui.tableView->columnWidth(1)<<"+"<<ui.tableView->columnWidth(2)<<"+"<<ui.tableView->columnWidth(3);
}

void IPWindow::on_pb_close_clicked()
{
    //close OK
    //emit refreshList();
    hide();
}

void IPWindow::on_pushButton_ipConf_clicked()
{//configurator
    //parent->mainBs->ipConfiguratorWindow->setParent(this);
    parent->ipConfiguratorWindow->IShow(dbIndex);
    //ipConfiguratorWindow->key_login=key_login;
    //ipConfiguratorWindow->iShow(dbIndex);
}

void IPWindow::on_pb_notes_clicked()
{
    notedlg->setTitle(n_name);
    notedlg->setNotes(notes);
    notedlg->iShow();
}

/*void IPWindow::on_pushButton_11_clicked()
{
  //open door OK

    if(!master->deviceDescr.data_mode)
        emit putCommand(dbIndex,"4",4);
    else
    {
        QSqlQuery* tq=new QSqlQuery();
        QString str;
        int in_cmd=4;
        
        COMM_STR.clear();
        COMM_STR.append("UC=4;");
        parent->COMM_STR.clear();
        parent->COMM_STR.append(master->deviceDescr.phone+"#"+COMM_STR);
            
        str.clear();
        str=str+"values ("+QString::number(dbIndex,10)+","+QString::number(in_cmd,10)+", "+"NOW(), "+"NOW(), "+"12, "+QString::number(in_cmd,10)+")";
        tq->exec("insert into send (device_id, command, time_seting, time_send, command_state, command_type) "+str);
        
        emit signalAllToServer();
        
        delete tq;
    }
    //emit putCommand(dbIndex,4,4);

}*/

void IPWindow::on_pb_state_clicked()
{//запрос состояния и времени
    emit putCommand(dbIndex,(master->deviceDescr.data_mode&&CHECK_BIT(buttons_config,1))?"ST=?;TM=?;":"5",5+50*master->deviceDescr.data_mode*CHECK_BIT(buttons_config,1));
}

void IPWindow::on_pb_counter_clicked()
{
        if(key_solut)
    {
        int ret = QMessageBox::warning(this, tr("Предупреждение системы"), 
                      tr("Вы действительно хотите выполнить команду?"), 
                      QMessageBox::Yes | QMessageBox::No); 
        if (ret != QMessageBox::Yes) return;
    }
    emit putCommand(dbIndex,master->deviceDescr.data_mode?"CNT1A=?;CNT2A=?;CNT1RP=?;CNT2RP=?;CNP=?;":"7",7+50*master->deviceDescr.data_mode);
}

void IPWindow::on_pb_sendtime_clicked()
{//time
    QString str;
    int normalDiff =parent->mainBs->settings->value("System/normal_UTC_diff",3).toInt();
    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime utcTime = currentTime.toUTC();
    utcTime.setTimeSpec(Qt::LocalTime);
    uint difTime = currentTime.toTime_t() - utcTime.toTime_t();
    int delta_time=difTime-normalDiff*60*60;
    delta_time=delta_time/3600;
    currentTime.setTime_t(currentTime.toTime_t() - delta_time*60*60);
    str.clear();
    str.append(currentTime.toString("yy/MM/dd,hh:mm:ss"));

    if(key_solut)
    {
        int ret = QMessageBox::warning(this, tr("Предупреждение системы"), 
                      tr("Вы действительно хотите выполнить команду?"), 
                      QMessageBox::Yes | QMessageBox::No); 
        if (ret != QMessageBox::Yes) return;
    }
    emit putCommand(dbIndex,(master->deviceDescr.data_mode&&CHECK_BIT(buttons_config,4))?"TM="+str+";":"0",0+50*master->deviceDescr.data_mode*CHECK_BIT(buttons_config,4));
}

void IPWindow::on_pb_par_clicked()
{//отправить время и перейти в ПАР
    QString time_str;
    int normalDiff = parent->mainBs->settings->value("System/normal_UTC_diff",3).toInt();
    //int extendedmode = parent->mainBs->settings->value("IPWindow/extended_mode",0).toInt();
    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime utcTime = currentTime.toUTC();
    utcTime.setTimeSpec(Qt::LocalTime);
    uint difTime = currentTime.toTime_t() - utcTime.toTime_t();
    int delta_time=difTime-normalDiff*60*60;
    delta_time=delta_time/3600;
    currentTime.setTime_t(currentTime.toTime_t() - delta_time*60*60);
    time_str.clear();
    time_str.append("TM=");
    time_str.append(currentTime.toString("yy/MM/dd,hh:mm:ss"));
    time_str.append(";UC=12;");
    emit putCommand(dbIndex,(master->deviceDescr.data_mode&&CHECK_BIT(buttons_config,5))?time_str:"C",12+50*master->deviceDescr.data_mode*CHECK_BIT(buttons_config,5));
}

void IPWindow::on_pb_requesttime_clicked()
{//запрос времени
    emit putCommand(dbIndex,(master->deviceDescr.data_mode&&CHECK_BIT(buttons_config,2))?"TM=?;":"3",3+50*master->deviceDescr.data_mode*CHECK_BIT(buttons_config,2));
}

void IPWindow::on_pbDelQueryCommands_clicked()
{
    //удалить (пометить как удаленные) неотправленные команды OK
    QSqlQuery *sendQuery = new QSqlQuery("update send set command_state=10 "
        "where command_state!=0 and command_state!=9 and command_state!=11 and command_state!=13 and device_id="+QString::number(dbIndex,10));
    emit signalToServer();
    delete sendQuery;
}

void IPWindow::on_pbMoreButtons_clicked()
{
    //additional OK
    parent->moreButtons->key_login=key_login;
    parent->moreButtons->key_anycommand=key_anycommand;
    parent->moreButtons->IShow(master);
}

void IPWindow::on_pbK1_clicked()
{
    //1
    if (k1==0){
        k1 = 1;
        ui.pbK1->setIcon(QIcon("./Resources/k1On.png"));
        //ui.pbK1->setIcon(QIcon(":/monitor/Resources/k1On.png"));
    }else{
        k1 = 0;
        ui.pbK1->setIcon(QIcon("./Resources/k1Off.png"));
        //ui.pbK1->setIcon(QIcon(":/monitor/Resources/k1Off.png"));
    }
}

void IPWindow::on_pbK2_clicked()
{
    //2
    if (k2==0){
        k2 = 1;
        ui.pbK2->setIcon(QIcon("./Resources/k2On.png"));
        //ui.pbK2->setIcon(QIcon(":/monitor/Resources/k2On.png"));
    }else{
        k2 = 0;
        ui.pbK2->setIcon(QIcon("./Resources/k2Off.png"));
        //ui.pbK2->setIcon(QIcon(":/monitor/Resources/k2Off.png"));
    }
}

void IPWindow::on_pbSendManualCmd_clicked()
{
    QString str;
    int cmd=0;

    master->updateDevice();
    master->updateConfig();
    if(key_solut)
    {
        int ret = QMessageBox::warning(this, tr("Предупреждение системы"), 
                      tr("Вы действительно хотите выполнить команду?"), 
                      QMessageBox::Yes | QMessageBox::No); 
        if (ret != QMessageBox::Yes) return;
    }

    if (k1==0)
    {//k1=0
        if (k2==0)
        {//0 0
            cmd = 8;
            str.clear();
        }
        else
        {//0 1
            cmd = 10;
            str.clear();
            }
    }
    else
    {//k1=1
        if (k2==0)
        {//1 0
            cmd = 9;
            str.clear();
        }
        else
        {//1 1
            cmd = 11;
            str.clear();
        }
    }

    emit putCommand(dbIndex,(master->deviceDescr.data_mode&&CHECK_BIT(buttons_config,3))?"UC="+QString::number(cmd,10)+";":QString::number(cmd,16),cmd+50*master->deviceDescr.data_mode*CHECK_BIT(buttons_config,3));

    master->updateState();
    master->updateMainIcon();
    master->updateImages();
}
    
void IPWindow::setAccessLevel(){
    QList<QObject *> childrenList = findChildren<QObject *>();
    QSqlQuery tq;
    QWidget *tW;
    QAction *tA;
    QString str;
    BaseSettings *mainBs = BaseSettings::createInstance();
    for (int i=0;i<childrenList.count();i++){
        if (childrenList.at(i)->objectName() != ""){
            //"monitor_Monitor_"+childrenList.at(i)->objectName() 
            str.clear();
            str.append("select is_enabled from roles_access where name = 'monitor_IPWindow_"+childrenList.at(i)->objectName()
                +"' and role = "+QString::number(mainBs->accessLevel,10));
            if (!tq.exec("select is_enabled from roles_access where name = 'monitor_IPWindow_"+
                childrenList.at(i)->objectName()+
                "' and role = "+QString::number(mainBs->accessLevel,10))){
                    //error
                    QSqlError err = tq.lastError();
                    QMessageBox::critical(0, tr("Ошибка выполнения запроса"),
                        err.text(), QMessageBox::Cancel,
                        QMessageBox::NoButton);
            }else{
                //ok
                if (tq.next()){
                    tW = dynamic_cast<QWidget*>(childrenList.at(i));
                    if (tW){
                        tW->setDisabled(!tq.value(tq.record().indexOf("is_enabled")).toBool());
                    }else{
                        tA = dynamic_cast<QAction*>(childrenList.at(i));
                        if (tA){
                            tA->setDisabled(!tq.value(tq.record().indexOf("is_enabled")).toBool());
                        }
                    }
                }
            }
        }
    }
    mainBs->freeInst();
}

void IPWindow::adrr_click()
{
    int a=0;
}

void IPWindow::DeltaEdit(IPObject* master,QDate date_cur,int num)
{
    int hour,min;
    qint16 realMinutes;
    hour=parent->Event_Table.Event[date_cur.month()-1][date_cur.day()-1][num].H;
    min=parent->Event_Table.Event[date_cur.month()-1][date_cur.day()-1][num].M;

    int D=0,S=0,K=0,h_d,m_d;
    switch (num)
    {
    case 0:
        D=master->deviceDescr.delta_1;
        break;
    case 1:
        D=master->deviceDescr.delta_2;
        break;
    case 2:
        D=master->deviceDescr.delta_3;
        break;
    case 3:
        D=master->deviceDescr.delta_4;
        break;
    default:
        D=0;
        break;
    }

    
    if (master->deviceDescr.data_mode)
    {
        realMinutes=hour*60+min+D;                  //D - заданное смещение расписания в минутах, realMinutes - это часы переведённые в минуты + минуты текущего шага и + смещение в ту или иную сторону
        if (realMinutes>=1440) realMinutes-=1440;   //если мы вылезли за 24 часа вперёд, считаем что начались новые сутки и отнимаем 24 часа в минутах
        if (realMinutes<0) realMinutes+=1440;       //если назад вылезли - считаем от конца предыдущих суток
        hour=realMinutes/60;                        //целое деление от новых realMinutes будет часы
        min=realMinutes%60;                         //остаток - минуты        
    }
    else
    {
        if(D>60)
        {
            S=D-60;
            hour=hour+S;
            if(hour>23) hour=hour-24;
            else if (hour<0) hour=hour+24;
            D=0;
        }

        if(D<-60)
        {
            S=D+60;
            K=hour;
            K=K+S;
            if (K<0)
                K=K+24;
            hour=K; 
            D=0;
        }
        if(D>0)
        {
            min=min+D;
            if(min>59)
            {
                min=min-60;
                if(hour==24)
                    hour=0;
                else    hour++;
            }
        }
        else
        {
            if(min>=abs(D))
                min=min-abs(D);
            else    
            {
                min=min+60-abs(D);
                if(hour==0)
                    hour=23;
                else    hour--; 
            }
        }
        
    }
    parent->Event_Table1.Event[date_cur.month()-1][date_cur.day()-1][num].H=hour;
    parent->Event_Table1.Event[date_cur.month()-1][date_cur.day()-1][num].M=min;
}

bool IPWindow::checkTime()
{
    bool key=false;

    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime utcTime = currentTime.toUTC();
    utcTime.setTimeSpec(Qt::LocalTime);
    int normalDiff = parent->mainBs->settings->value("System/normal_UTC_diff",2).toInt();;
    uint difTime = currentTime.toTime_t() - utcTime.toTime_t();
    
    if (difTime == normalDiff*60*60)
        key=false;
    else
        key=true;
    
    return key;
}

void IPWindow::pushButton_DeltaRequest_clicked()
{//запрос шагов основного ПАРа, дельт и дополнительного
    emit putCommand(dbIndex,"AUP=?;DL=?;AUS=?;",14);
}

void IPWindow::pushButton_MskRequest_clicked()
{//запрос маски и типа счётчика
    emit putCommand(dbIndex,"MSK=?;CNP=?;",21);
}
