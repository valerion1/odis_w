#include "additionalbuttons.h"
#include "datatypes.h"
#include "basesettings.h"
#include "ipwindow.h"
#include "monitor.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QTextStream>
#include <QFile>
#include <QSqlRecord>
#include <QDateTime>
#include <QTimer>
#include <QValidator>
#include <QRegExp>
#include <QSignalMapper>

AdditionalButtons::AdditionalButtons(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    /*this->setWindowModality(Qt::ApplicationModal);
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

    this->setFixedSize(500,212);
    */

    key_login=false;
    key_anycommand=false;
    QSettings *settings = new QSettings("monitor.ini", QSettings::IniFormat,this);
    //загрузка дельт 
    QTextStream text;
    QString *prefix=new QString("Buffer_Delta/pos");
    text.setString(prefix);
    for(int i=0;i<16;i++)
    {
        text<<i+1;
        DeltaArr[i]=settings->value(*text.string()).toInt();
        prefix->clear();
        prefix->append("Buffer_Delta/pos");
    }
    delete settings;
}

AdditionalButtons::~AdditionalButtons(){}

void AdditionalButtons::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        hide();
        this->destroy();
    }
}

void AdditionalButtons::setParent(IPWindow* ptr)
{
    parent=(IPWindow*)ptr;
    //connect(this,SIGNAL(reloadavt()),parent,SLOT(ReloadAvtonom()));
}

void AdditionalButtons::setParent2(monitor * ptr)
{
    parent2=(monitor*)ptr;
}

void AdditionalButtons::groupShow(DevList *in_devList)
{
    devList=in_devList;

    ui.pbRebootSoft->hide();
    ui.pbRebootHard->hide();
    ui.pushButtonAll->hide();
    ui.groupBoxService->hide();

    ui.pbGPRS_Soft->hide();
    ui.pbGPRS_Avtonom->hide();
    ui.pbGPRS_Stat->hide();
    ui.groupBoxGPRS->hide();

    ui.pbDeltaSend->hide();
    ui.pbDeltaSendToGroup->show();
    bool data_present=false;
    if (!devList->isEmpty())
        for (int i=0;i<devList->count();i++)
        {
            device_ptr=devList->at(i);
            if (device_ptr->deviceDescr.data_mode)
            {
                data_present=true;
                break;//для нормальной работы окна с группой необходимо найти любой прибор с датой и скормить функциям в качестве шаблона
            }
        }
    if (!data_present) 
    {
        this->hide();
        QMessageBox warn;
        warn.setText("Ошибка.\nВ списке не обнаружено приборов с расширенным режимом передачи команд. Продолжение работы этого окна невозможно. Придётся его закрыть. Если Вы считаете что так быть не должно, обратитесь, пожалуйста, в <a href=\"http://ow.od.ua/?page_id=93\">службу технической поддержки</a> и мы постараемся Вам помочь.");
        warn.setIcon(QMessageBox::Critical);
        warn.exec();
        return;
    }

    /*group_steps[0].num=0;	group_steps[0].act=1;	group_steps[0].time=QTime(0,0,0,0);	group_steps[0].knkv=10;	group_steps[0].delta=0;	group_steps[0].primary=1;	group_steps[0].changed=0;
    group_steps[1].num=1;	group_steps[1].act=1;	group_steps[1].time=QTime(0,0,0,0);	group_steps[1].knkv=10;	group_steps[1].delta=0;	group_steps[1].primary=1;	group_steps[1].changed=0;
    group_steps[2].num=2;	group_steps[2].act=1;	group_steps[2].time=QTime(0,0,0,0);	group_steps[2].knkv=10;	group_steps[2].delta=0;	group_steps[2].primary=1;	group_steps[2].changed=0;
    group_steps[3].num=3;	group_steps[3].act=1;	group_steps[3].time=QTime(0,0,0,0);	group_steps[3].knkv=10;	group_steps[3].delta=0;	group_steps[3].primary=1;	group_steps[3].changed=0;
    group_steps[4].num=12;	group_steps[4].act=1;	group_steps[4].time=QTime(0,0,0,0);	group_steps[4].knkv=10;	group_steps[4].delta=0;	group_steps[4].primary=0;	group_steps[4].changed=0;
    group_steps[5].num=13;	group_steps[5].act=1;	group_steps[5].time=QTime(0,0,0,0);	group_steps[5].knkv=10;	group_steps[5].delta=0;	group_steps[5].primary=0;	group_steps[5].changed=0;
    group_steps[6].num=14;	group_steps[6].act=1;	group_steps[6].time=QTime(0,0,0,0);	group_steps[6].knkv=10;	group_steps[6].delta=0;	group_steps[6].primary=0;	group_steps[6].changed=0;
    group_steps[7].num=15;	group_steps[7].act=1;	group_steps[7].time=QTime(0,0,0,0);	group_steps[7].knkv=10;	group_steps[7].delta=0;	group_steps[7].primary=0;	group_steps[7].changed=0;*/
    parent2->LoadAvtonom();
    //сгенерить всю таблицу циклом вместе с названиями чекбоксов
    //при вводе данные обрабатывать через одну функцию, передавать обьект как параметр
    refreshGroupParSteps(stepsADD);
    //redrawGroupGrid();//перерисовать сетку с шагами
    redrawParGrid(device_ptr);
    adjustSize();
    show();
    toDebug(getDeltaString(true));//data
}

void AdditionalButtons::refreshGroupParSteps(_step * in_group_steps)
{
    in_group_steps[0].num=0;	in_group_steps[0].act=parent2->avto.key[0];	in_group_steps[0].time=QTime::fromString(parent2->avto.tm[0],"hh:mm");	in_group_steps[0].knkv=parent2->avto.state[0];	in_group_steps[0].delta=0;						in_group_steps[0].primary=1;	in_group_steps[0].changed=0;
    in_group_steps[1].num=1;	in_group_steps[1].act=parent2->avto.key[1];	in_group_steps[1].time=QTime::fromString(parent2->avto.tm[1],"hh:mm");	in_group_steps[1].knkv=parent2->avto.state[1];	in_group_steps[1].delta=0;						in_group_steps[1].primary=1;	in_group_steps[1].changed=0;
    in_group_steps[2].num=2;	in_group_steps[2].act=parent2->avto.key[2];	in_group_steps[2].time=QTime::fromString(parent2->avto.tm[2],"hh:mm");	in_group_steps[2].knkv=parent2->avto.state[2];	in_group_steps[2].delta=0;						in_group_steps[2].primary=1;	in_group_steps[2].changed=0;
    in_group_steps[3].num=3;	in_group_steps[3].act=parent2->avto.key[3];	in_group_steps[3].time=QTime::fromString(parent2->avto.tm[3],"hh:mm");	in_group_steps[3].knkv=parent2->avto.state[3];	in_group_steps[3].delta=0;						in_group_steps[3].primary=1;	in_group_steps[3].changed=0;
    in_group_steps[4].num=12;	in_group_steps[4].act=0;					in_group_steps[4].time=QTime(0,0,0,0);									in_group_steps[4].knkv=10;	in_group_steps[4].delta=0;	in_group_steps[4].primary=0;	in_group_steps[4].changed=0;
    in_group_steps[5].num=13;	in_group_steps[5].act=0;					in_group_steps[5].time=QTime(0,0,0,0);									in_group_steps[5].knkv=10;	in_group_steps[5].delta=0;	in_group_steps[5].primary=0;	in_group_steps[5].changed=0;
    in_group_steps[6].num=14;	in_group_steps[6].act=0;					in_group_steps[6].time=QTime(0,0,0,0);									in_group_steps[6].knkv=10;	in_group_steps[6].delta=0;	in_group_steps[6].primary=0;	in_group_steps[6].changed=0;
    in_group_steps[7].num=15;	in_group_steps[7].act=0;					in_group_steps[7].time=QTime(0,0,0,0);									in_group_steps[7].knkv=10;	in_group_steps[7].delta=0;	in_group_steps[7].primary=0;	in_group_steps[7].changed=0;
}

void AdditionalButtons::IShow(IPObject * in_device_ptr){
    bool ok;
    QString str;
    device_ptr = in_device_ptr;
    //QSqlQuery *tq = new QSqlQuery();
    //секция рисования окна
    ui.pbGPRS_Avtonom->setDisabled(!key_login);
    ui.pbGPRS_Soft->setDisabled(!key_login);

    ui.pbRebootSoft->show();
    ui.pbRebootHard->show();
    ui.pushButtonAll->show();
    ui.groupBoxService->show();

    ui.pbGPRS_Soft->show();
    ui.pbGPRS_Avtonom->show();
    ui.pbGPRS_Stat->show();
    ui.groupBoxGPRS->show();

    ui.pbDeltaSend->show();
    ui.pbDeltaSendToGroup->hide();
    //сгенерить всю таблицу циклом вместе с названиями чекбоксов
    //при вводе данные обрабатывать через одну функцию, передавать обьект как параметр

    //QCheckBox * par_act[8];
    //PARButtons * par_buttons[8];
    //QPushButton * par_kn[8],par_kv[8];
    //QSignalMapper *_signalMapper= new QSignalMapper(this);
    //debugLabel=new QLabel(this);
    //ui.delta_VBoxLayout->addWidget(debugLabel);
    parent->refreshParSteps(stepsADD);//записать во внутренний массив всю информацию по шагам вместе с сортировкой
    redrawParGrid(device_ptr);//перерисовать сетку с шагами
    adjustSize();
    show();
    toDebug(getDeltaString(true));//data
}//IShow end

void AdditionalButtons::redrawParGrid(IPObject * in_device)
{
    int _grid_x=0,_grid_y=0,rez=0;
    QPixmap pixmap;
    QLayoutItem * temp_item;
    while ((temp_item = ui.delta_GridLayout->takeAt(0)) != 0)
    {
        delete temp_item->widget();
        delete temp_item;
    }

    for (int i=0;i<8;i++)
    {
    //rez=(!parent->master->deviceDescr.data_mode)*(!stepsADD[i].primary)*(!stepsADD[i].act)*0 + (!parent->master->deviceDescr.data_mode)*(!stepsADD[i].primary)*stepsADD[i].act*0 + (!parent->master->deviceDescr.data_mode)*stepsADD[i].primary*(!stepsADD[i].act)*0 + (!parent->master->deviceDescr.data_mode)*stepsADD[i].primary*stepsADD[i].act*3 + parent->master->deviceDescr.data_mode*(!stepsADD[i].primary)*(!stepsADD[i].act)*1 + parent->master->deviceDescr.data_mode*(!stepsADD[i].primary)*stepsADD[i].act*2 + parent->master->deviceDescr.data_mode*stepsADD[i].primary*(!stepsADD[i].act)*0 + parent->master->deviceDescr.data_mode*stepsADD[i].primary*stepsADD[i].act*4;
    rez=(!in_device->deviceDescr.data_mode)*(!stepsADD[i].changed)*(!stepsADD[i].primary)*(!stepsADD[i].act)*0 + 
        (!in_device->deviceDescr.data_mode)*(!stepsADD[i].changed)*(!stepsADD[i].primary)*stepsADD[i].act*0 + 
        (!in_device->deviceDescr.data_mode)*(!stepsADD[i].changed)*stepsADD[i].primary*(!stepsADD[i].act)*0 + 
        (!in_device->deviceDescr.data_mode)*(!stepsADD[i].changed)*stepsADD[i].primary*stepsADD[i].act*3 + 
        in_device->deviceDescr.data_mode*(!stepsADD[i].changed)*(!stepsADD[i].primary)*(!stepsADD[i].act)*1 + 
        in_device->deviceDescr.data_mode*(!stepsADD[i].changed)*(!stepsADD[i].primary)*stepsADD[i].act*2 + 
        in_device->deviceDescr.data_mode*(!stepsADD[i].changed)*stepsADD[i].primary*(!stepsADD[i].act)*0 + 
        in_device->deviceDescr.data_mode*(!stepsADD[i].changed)*stepsADD[i].primary*stepsADD[i].act*4 + 
        in_device->deviceDescr.data_mode*stepsADD[i].changed*(!stepsADD[i].primary)*(!stepsADD[i].act)*1 + 
        in_device->deviceDescr.data_mode*stepsADD[i].changed*(!stepsADD[i].primary)*stepsADD[i].act*5 + 
        in_device->deviceDescr.data_mode*stepsADD[i].changed*stepsADD[i].primary*0;

    //rez=1;
    QIntValidator * int_validator = new QIntValidator(-1440,1440,this);
    switch (rez)
    {
    case 0: break;
    case 1://дополнительный шаг не активен, для редактирования нажимаем чекбокс (попадаем в 5)
            _grid_x=0;//место для состояния контакторов
            par_kn[i]= new CPushButton(this,i,stepsADD[i].num,1);
            par_kn[i]->setGeometry(0,0,5,5);
            par_kn[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            par_kn[i]->setEnabled(false);
            par_kn[i]->setCheckable(true);
            par_kn[i]->setChecked(false);
            par_kv[i]= new CPushButton(this,i,stepsADD[i].num,0);
            par_kv[i]->setGeometry(0,0,5,5);
            par_kv[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            par_kv[i]->setEnabled(false);
            par_kv[i]->setCheckable(true);
            par_kv[i]->setChecked(false);
            connect(par_kn[i],SIGNAL(toggled(bool)),par_kn[i],SLOT(knChanged(bool)));
            connect(par_kn[i],SIGNAL(customToggled(bool,int,bool)),this,SLOT(knkvToggled(bool,int,bool)));
            connect(par_kv[i],SIGNAL(toggled(bool)),par_kv[i],SLOT(kvChanged(bool)));
            connect(par_kv[i],SIGNAL(customToggled(bool,int,bool)),this,SLOT(knkvToggled(bool,int,bool)));
            par_kn[i]->setIcon(QIcon("./Resources/k1Off.png"));
            par_kv[i]->setIcon(QIcon("./Resources/k2Off.png"));
            ui.delta_GridLayout->addWidget(par_kn[i],_grid_y,_grid_x);
            ui.delta_GridLayout->addWidget(par_kv[i],_grid_y,_grid_x+1);
            //-------------------------------------------------------------
            _grid_x=2;//место для времени+дельта
            par_time[i]= new CTimeEdit(this,i,stepsADD[i].num,stepsADD[i].time);//00:00
            par_time[i]->setMinimumSize(52,10);par_time[i]->setMaximumSize(65,30);
            par_time[i]->setDisplayFormat("hh:mm");
            //par_time[i]->setTime(steps[i].time.addSecs(steps[i].delta*60+1));
            par_time[i]->setTime(QTime(0,0,0,0));
            par_time[i]->setEnabled(false);
            par_time[i]->setObjectName("par_time_"+QString::number(i));
            par_time[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            connect(par_time[i],SIGNAL(timeChanged(QTime)),par_time[i],SLOT(updateDelta(QTime)));
            connect(par_time[i],SIGNAL(secDeltaChanged(QTime,int)),this,SLOT(updateSecondaryDelta(QTime,int)));
            ui.delta_GridLayout->addWidget(par_time[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_x=3;
            par_act[i]=new CCheckBox(this,i,stepsADD[i].num);
            par_act[i]->setChecked(false);
            par_act[i]->setText(tr("шаг выкл"));
            par_act[i]->setObjectName("par_act_"+QString::number(i));
            connect(par_act[i],SIGNAL(stateChanged(int)),par_act[i],SLOT(ccboxStateChanged(int)));
            connect(par_act[i],SIGNAL(cboxStateChanged(int,int,bool)),this,SLOT(changeAdditionalStep(int,int,bool)));
            ui.delta_GridLayout->addWidget(par_act[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_y++;
            break;
    case 2://дополнительный шаг активен, редактировать нельзя, можно только удалить сняв чекбокс (попадаем в 6)
            _grid_x=0;//место для состояния контакторов
            par_event[i]= new QLabel(this);//создаём текстовую/графическую метку для лампочек
            //par_event[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            //par_event[i]->setGeometry(1,1,58,24);//масштабируем       //!!!задать параметры геометрии
            //par_event[i]->resize(58,24);
            switch(stepsADD[i].knkv)
            {
                case 10:pixmap.load("./Resources/auto_0_0.png");break;
                case 11:pixmap.load("./Resources/auto_1_0.png");break;
                case 14:pixmap.load("./Resources/auto_0_1.png");break;
                case 15:pixmap.load("./Resources/auto_1_1.png");break;
            }
            pixmap=pixmap.scaledToWidth(58);
            pixmap=pixmap.scaledToHeight(24);
            par_event[i]->setPixmap(pixmap);//рисуем лампочки
            ui.delta_GridLayout->addWidget(par_event[i],_grid_y,_grid_x,1,2);
            //-------------------------------------------------------------
            _grid_x=2;//место для времени+дельта
            par_time[i]= new CTimeEdit(this,i,stepsADD[i].num,stepsADD[i].time);//создаём текстовую/графическую метку для лампочек
            par_time[i]->setMinimumSize(52,10);par_time[i]->setMaximumSize(65,30);
            par_time[i]->setDisplayFormat("hh:mm");
            par_time[i]->setTime(stepsADD[i].time.addSecs(stepsADD[i].delta*60+monitor::getTimeDifference()*60));//переводим с времени терминала в локальное на компе
            par_time[i]->setEnabled(false);
            par_time[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            ui.delta_GridLayout->addWidget(par_time[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_x=3;
            par_act[i]=new CCheckBox(this,i,stepsADD[i].num);
            par_act[i]->setChecked(true);
            par_act[i]->setText(tr("шаг   вкл"));
            connect(par_act[i],SIGNAL(stateChanged(int)),par_act[i],SLOT(ccboxStateChanged(int)));
            connect(par_act[i],SIGNAL(cboxStateChanged(int,int,bool)),this,SLOT(changeAdditionalStep(int,int,bool)));
            ui.delta_GridLayout->addWidget(par_act[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_y++;
            break;
    case 3://основной автоном голос, активный шаг - нарисовать лампы, нарисовать время без редактирования, дельту с выпадающим списком без редактирования
            _grid_x=0;//место для состояния контакторов
            par_event[i]= new QLabel(this);//создаём текстовую/графическую метку для лампочек
            //par_event[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            //par_event[i]->setGeometry(1,1,58,24);//масштабируем       //!!!задать параметры геометрии
            //par_event[i]->resize(58,24);
            switch(stepsADD[i].knkv)
            {
                case 10:pixmap.load("./Resources/auto_0_0.png");break;
                case 11:pixmap.load("./Resources/auto_1_0.png");break;
                case 14:pixmap.load("./Resources/auto_0_1.png");break;
                case 15:pixmap.load("./Resources/auto_1_1.png");break;
            }
            pixmap=pixmap.scaledToWidth(58);
            pixmap=pixmap.scaledToHeight(24);
            par_event[i]->setPixmap(pixmap);//рисуем лампочки
            ui.delta_GridLayout->addWidget(par_event[i],_grid_y,_grid_x,1,2);
            //-------------------------------------------------------------
            _grid_x=2;//место для времени+дельта
            par_time[i]= new CTimeEdit(this,i,stepsADD[i].num,stepsADD[i].time);//создаём текстовую/графическую метку для лампочек
            par_time[i]->setMinimumSize(52,10);par_time[i]->setMaximumSize(65,30);
            par_time[i]->setDisplayFormat("hh:mm");
            par_time[i]->setTime(stepsADD[i].time.addSecs(stepsADD[i].delta*60+monitor::getTimeDifference()*60));//переводим с времени терминала + дельта в докальное на компе + дельта
            par_time[i]->setEnabled(false);
            par_time[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            ui.delta_GridLayout->addWidget(par_time[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_x=3;
            par_delta[i]=new CComboBox(this,i,stepsADD[i].num);
            par_delta[i]->setEditable(false);
            par_delta[i]->setCurrentIndex(0);
            for(int j=0;j<16;j++)
            {
                //str.append(QString::number(DeltaArr[j]));
                par_delta[i]->addItem(QString::number(DeltaArr[j]));
                if (stepsADD[i].delta==DeltaArr[j]) par_delta[i]->setCurrentIndex(j);
            }
            par_delta[i]->setObjectName("par_delta_"+QString::number(i));
            //par_delta[i]->installEventFilter(this);//при изменении дельты пересчитаем время в окошке
            //connect(par_delta[i],SIGNAL(editTextChanged(QString)),par_delta[i],SLOT(deltaChanged(QString)));
            connect(par_delta[i],SIGNAL(currentIndexChanged(QString)),par_delta[i],SLOT(deltaChanged(QString)));
            connect(par_delta[i],SIGNAL(changeTime(int,int)),this,SLOT(timeChange(int,int)));
            ui.delta_GridLayout->addWidget(par_delta[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_y++;
            break;
    case 4://основной автоном дата, активный шаг - нарисовать лампы, нарисовать время без редактирования, дельту с редактированием
            _grid_x=0;//место для состояния контакторов
            par_event[i]= new QLabel(this);//создаём текстовую/графическую метку для лампочек
            //par_event[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            //par_event[i]->setGeometry(1,1,58,24);//масштабируем       //!!!задать параметры геометрии
            //par_event[i]->resize(58,24);
            switch(stepsADD[i].knkv)
            {
                case 10:pixmap.load("./Resources/auto_0_0.png");break;
                case 11:pixmap.load("./Resources/auto_1_0.png");break;
                case 14:pixmap.load("./Resources/auto_0_1.png");break;
                case 15:pixmap.load("./Resources/auto_1_1.png");break;
            }
            pixmap=pixmap.scaledToWidth(58);
            pixmap=pixmap.scaledToHeight(24);
            par_event[i]->setPixmap(pixmap);//рисуем лампочки
            ui.delta_GridLayout->addWidget(par_event[i],_grid_y,_grid_x,1,2);
            //-------------------------------------------------------------
            _grid_x=2;//место для времени+дельта
            par_time[i]= new CTimeEdit(this,i,stepsADD[i].num,stepsADD[i].time);//создаём текстовую/графическую метку для лампочек
            par_time[i]->setMinimumSize(52,10);par_time[i]->setMaximumSize(65,30);
            par_time[i]->setDisplayFormat("hh:mm");
            par_time[i]->setTime(stepsADD[i].time.addSecs(stepsADD[i].delta*60+monitor::getTimeDifference()*60));//переводим с времени терминала + дельта в локальное компа + дельта
            par_time[i]->setEnabled(false);
            par_time[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            ui.delta_GridLayout->addWidget(par_time[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_x=3;
            par_delta[i]=new CComboBox(this,i,stepsADD[i].num);
            par_delta[i]->setEditable(true);
            for(int j=0;j<16;j++)
            {
                //str.append(QString::number(DeltaArr[j]));
                par_delta[i]->addItem(QString::number(DeltaArr[j]));
                if (stepsADD[i].delta==DeltaArr[j]) par_delta[i]->setCurrentIndex(j);
            }
            //QValidator *validator = new QRegExpValidator(rx, this);
            par_delta[i]->lineEdit()->setValidator(int_validator);
            par_delta[i]->setEditText(QString::number(stepsADD[i].delta));
            par_delta[i]->setObjectName("par_delta_"+QString::number(i));
            par_delta[i]->setAutoCompletion(false);
            //par_delta[i]->installEventFilter(this);//при изменении дельты пересчитаем время в окошке
            connect(par_delta[i],SIGNAL(editTextChanged(QString)),par_delta[i],SLOT(deltaChanged(QString)));
            connect(par_delta[i],SIGNAL(changeTime(int,int)),this,SLOT(timeChange(int,int)));
            ui.delta_GridLayout->addWidget(par_delta[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_y++;
            break;
    case 5://дополнительный шаг сделали активным, нажали чекбокс и перерисовали (попали в 5)
            _grid_x=0;//место для состояния контакторов
            par_kn[i]= new CPushButton(this,i,stepsADD[i].num,1);
            par_kn[i]->setGeometry(0,0,5,5);
            par_kn[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            par_kn[i]->setEnabled(true);
            par_kn[i]->setCheckable(true);
            par_kn[i]->setChecked(stepsADD[i].knkv&(1<<0));//проверили как стоял контактор КН в массиве шагов
            par_kn[i]->setIcon((stepsADD[i].knkv&(1<<0))?QIcon("./Resources/k1On_m.png"):QIcon("./Resources/k1Off_m.png"));
            par_kv[i]= new CPushButton(this,i,stepsADD[i].num,0);
            par_kv[i]->setGeometry(0,0,5,5);
            par_kv[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            par_kv[i]->setEnabled(true);
            par_kv[i]->setCheckable(true);
            par_kv[i]->setChecked(stepsADD[i].knkv&(1<<2));
            par_kv[i]->setIcon((stepsADD[i].knkv&(1<<2))?QIcon("./Resources/k2On_m.png"):QIcon("./Resources/k2Off_m.png"));
            connect(par_kn[i],SIGNAL(toggled(bool)),par_kn[i],SLOT(knChanged(bool)));
            connect(par_kn[i],SIGNAL(customToggled(bool,int,bool)),this,SLOT(knkvToggled(bool,int,bool)));
            connect(par_kv[i],SIGNAL(toggled(bool)),par_kv[i],SLOT(kvChanged(bool)));
            connect(par_kv[i],SIGNAL(customToggled(bool,int,bool)),this,SLOT(knkvToggled(bool,int,bool)));
            ui.delta_GridLayout->addWidget(par_kn[i],_grid_y,_grid_x);
            ui.delta_GridLayout->addWidget(par_kv[i],_grid_y,_grid_x+1);
            //-------------------------------------------------------------
            _grid_x=2;//место для времени+дельта
            par_time[i]= new CTimeEdit(this,i,stepsADD[i].num,stepsADD[i].time);//00:00
            par_time[i]->setMinimumSize(52,10);par_time[i]->setMaximumSize(65,30);
            par_time[i]->setDisplayFormat("hh:mm");
            par_time[i]->setTime(stepsADD[i].time.addSecs(stepsADD[i].delta*60+monitor::getTimeDifference()*60));//переводим с времени терминала в локальное на компе
            //par_time[i]->setTime(QTime(0,0,0,0));
            par_time[i]->setEnabled(true);
            par_time[i]->setObjectName("par_time_"+QString::number(i));
            par_time[i]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
            connect(par_time[i],SIGNAL(timeChanged(QTime)),par_time[i],SLOT(updateDelta(QTime)));
            connect(par_time[i],SIGNAL(secDeltaChanged(QTime,int)),this,SLOT(updateSecondaryDelta(QTime,int)));
            ui.delta_GridLayout->addWidget(par_time[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_x=3;
            par_act[i]=new CCheckBox(this,i,stepsADD[i].num);
            par_act[i]->setChecked(true);
            par_act[i]->setText(tr("шаг   вкл"));
            par_act[i]->setObjectName("par_act_"+QString::number(i));
            connect(par_act[i],SIGNAL(stateChanged(int)),par_act[i],SLOT(ccboxStateChanged(int)));
            connect(par_act[i],SIGNAL(cboxStateChanged(int,int,bool)),this,SLOT(changeAdditionalStep(int,int,bool)));
            ui.delta_GridLayout->addWidget(par_act[i],_grid_y,_grid_x);
            //-------------------------------------------------------------
            _grid_y++;
            //stepsADD[i].act=true;//учитываем что поставили в чекбоксе в генерации строки
            break;
        }
    }//for
}

void AdditionalButtons::on_pbRebootSoft_clicked()
{   //1
    //emit putCommand(device_ptr->rec_id,parent->master->deviceDescr.data_mode?"UC=1;":"1",1+50*parent->master->deviceDescr.data_mode);
    emit putCommand(device_ptr->rec_id,(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6))?"UC=1;":"1",1+50*device_ptr->deviceDescr.data_mode*CHECK_BIT(parent->buttons_config,6));
    /*if(!parent->master->deviceDescr.data_mode)
        emit putCommand(devID,"1",1);
    else
    {
        QString str;
        str.clear();
        str.append("UC=1;");
        parent->SendData(str,1);
    }
    //emit emitCmd(devID,1,1);*/
}

void AdditionalButtons::on_pbRebootHard_clicked()
{//15
    //emit putCommand(devID,parent->master->deviceDescr.data_mode?"UC=15;":"F",15+50*parent->master->deviceDescr.data_mode);
    emit putCommand(device_ptr->rec_id,device_ptr->deviceDescr.data_mode?"UC=15;":"F",15+50*device_ptr->deviceDescr.data_mode*CHECK_BIT(parent->buttons_config,6));
    /*if(!parent->master->deviceDescr.data_mode)
        emit putCommand(devID,"F",15);
    else
    {
        QString str;
        str.clear();
        str.append("UC=15;");
        parent->SendData(str,15);
    }
    //emit emitCmd(devID,15,15);*/
}

void AdditionalButtons::on_pbDeltaSend_clicked()//сгенерить строку для даты/сгенерить команду для голоса по дельте
{
    //emit putCommand(devID,getDeltaString(parent->master->deviceDescr.data_mode),18+50*parent->master->deviceDescr.data_mode);
    emit putCommand(device_ptr->rec_id,getDeltaString(device_ptr->deviceDescr.data_mode),18+50*device_ptr->deviceDescr.data_mode);
/*    if(parent->master->deviceDescr.data_mode)
    {
        parent->SendData(getDeltaString(),18);
    }//data
    else
    {//voice
        emit emitCmd(devID,getVoiceDeltaCommand(),18);
    }
*/
    hide();
    this->destroy();
}

void AdditionalButtons::on_pbDeltaSendToGroup_clicked()//сгенерить строку для даты/сгенерить команду для голоса по дельте
{
    QString warning_voice_str;
    IPObject * tmp_dev;
    warning_voice_str.clear();
    while (!devList->isEmpty())
    {
        tmp_dev = devList->takeFirst();
        if (tmp_dev->deviceDescr.data_mode)
        {
            emit putCommand(tmp_dev->rec_id,getDeltaString(true),18+50);
        }
        else
        {
            warning_voice_str+="   "+tmp_dev->deviceDescr.name+", "+tmp_dev->deviceDescr.phone+"<br>";
            toDebug("voice device: "+QString::number(tmp_dev->rec_id,10)+", name: "+tmp_dev->deviceDescr.name+", phone: "+tmp_dev->deviceDescr.phone);
        }
        delete tmp_dev;
    }
    if (!warning_voice_str.isEmpty())
    {
        QMessageBox warning_voice;
        warning_voice.setText(tr("В группе обнаружены приборы, которые не поддерживают установку дополнительных шагов и задание произвольных смещений основных шагов ПАР. На эти приборы команда отправляться не будет. Рекомендуется выяснить возможность их перенастройки в <a href=\"http://ow.od.ua/?page_id=93\">сервисном центре производителя</a>. До этого времени единственным выходом из сложившейся ситуации является отправка смещений шагов ПАР индивидуально на следующий список:<br><br>")+warning_voice_str);
        warning_voice.setIcon(QMessageBox::Warning);
        warning_voice.exec();
    }

    hide();
    this->destroy();
}

void AdditionalButtons::on_pbGPRS_Soft_clicked()
{//FFF01    //19
    emit putCommand(device_ptr->rec_id,(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6))?"UC=121;":"FFF01",19+50*(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6)));
    /*if(!parent->master->deviceDescr.data_mode)
        emit putCommand(devID,"FFF01",19);
    else
    {
        QString str;
        
        str.clear();
        str.append("UC=121;");
        parent->SendData(str,19);
    }
    //emit emitCmd(devID,0xfff01,19);*/
        hide();
        destroy();
}

void AdditionalButtons::on_pbGPRS_Avtonom_clicked()
{//FFF02    //20
    emit putCommand(device_ptr->rec_id,(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6))?"UC=122;":"FFF02",20+50*(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6)));
    /*if(!parent->master->deviceDescr.data_mode)
        emit putCommand(devID,"FFF02",20);
    else
    {
        QString str;
        
        str.clear();
        str.append("UC=122;");
        parent->SendData(str,20);
    }
    //emit emitCmd(devID,0xfff02,20);*/
}

void AdditionalButtons::on_pbGPRS_Stat_clicked()
{//FFF03    //16
    emit putCommand(device_ptr->rec_id,(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6))?"UC=123;":"FFF03",16+50*(device_ptr->deviceDescr.data_mode&&CHECK_BIT(parent->buttons_config,6)));
    /*if(!parent->master->deviceDescr.data_mode)
        emit putCommand(devID,"FFF03",16);
    else
    {
        QString str;
        
        str.clear();
        str.append("UC=123;");
        parent->SendData(str,16);
    }
    //emit emitCmd(devID,0xfff03,16);*/
}

void AdditionalButtons::on_pbClose_clicked()
{
    hide();
    this->destroy();
    //this=0;
}

void AdditionalButtons::setAccessLevel(){
    /*QList<QObject *> childrenList = findChildren<QObject *>();
    QSqlQuery tq;
    QWidget *tW;
    QAction *tA;
    BaseSettings *mainBs = BaseSettings::createInstance();
    for (int i=0;i<childrenList.count();i++){
        if (childrenList.at(i)->objectName() != ""){
            //"monitor_Monitor_"+childrenList.at(i)->objectName() 
            if (!tq.exec("select is_enabled from roles_access where name = 'monitor_AdditionalButtons_"+
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
    mainBs->freeInst();*/
}

//----------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------------
CTimeEdit::CTimeEdit(QWidget *parent,int _row,int _step_num, QTime _time) : QTimeEdit(parent)
{
    step_number=_step_num;
    row=_row;
    rawtime=_time;
}
void CTimeEdit::updateDelta(QTime _time) //передаёт номер шага и значение в updateDeltas
{
    //AdditionalButtons::sec_delta[0].time=QTime(0,0,0,0);
    //this->step_number
    //AdditionalButtons.setSecondaryDeltaTime(this->step_number,_time);
    emit secDeltaChanged(_time,this->row);
}
//----------------------------------------------------------------------------------------------------------------------------------------

CCheckBox::CCheckBox(QWidget *parent,int _row,int _step_num) : QCheckBox(parent)//активность дополнительного шага
{
    step_number=_step_num;
    row=_row;
}

void CCheckBox::ccboxStateChanged(int state)
{
    emit cboxStateChanged(state,this->row,!state);
}
/*void CCheckBox::actStateChanged(int state)
{
    emit cboxStateChanged(state,this->row,!state);
}*/
//----------------------------------------------------------------------------------------------------------------------------------------

CComboBox::CComboBox(QWidget *parent,int _row,int _step_num) : QComboBox(parent)//дельта
{
    step_number=_step_num;
    row=_row;
}
void CComboBox::deltaChanged(QString _textdelta)
{
    int _delta=_textdelta.toInt();
    emit changeTime(_delta,row);
}
//----------------------------------------------------------------------------------------------------------------------------------------
CPushButton::CPushButton(QWidget *parent,int _row,int _step_num,bool _kn) : QPushButton(parent)
{
    step_number=_step_num;
    row=_row;
    kn=_kn;
}

void CPushButton::knChanged(bool _checked)
{
    checked=_checked;
    emit customToggled(_checked,this->row,0);
}

void CPushButton::kvChanged(bool _checked)
{
    checked=_checked;
    emit customToggled(_checked,this->row,1);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void AdditionalButtons::changeAdditionalStep(int _state,int _row,bool _active)//_active=1 - он уже был, снимаем; _active=0 - его ещё не было, ставим;
{
    stepsADD[_row].changed=true;
    if (_active)//попали в 6 (сняли активный шаг)
    {
        stepsADD[_row].changed=true;
        stepsADD[_row].act=false;
        stepsADD[_row].knkv=0;
        stepsADD[_row].delta=0;
    }
    else
    {//попали в 5 (включили чекбокс, из неактивного шага сделали активный, но ещё не отправленный)
        stepsADD[_row].changed=true;
        stepsADD[_row].act=true;
        stepsADD[_row].knkv=10;
        stepsADD[_row].delta=0 - monitor::getTimeDifference();//перевод из часового пояса монитора в часовой пояс терминала (минуты), если диспетчер оставил 00:00, отправить верное для терминала время соответственно разнице часовых поясов
    }
    redrawParGrid(device_ptr);//мы НЕ МОЖЕМ запускать перерисовку, там нет "5" !!!
    toDebug(getDeltaString(true));//data
}

void AdditionalButtons::knkvToggled(bool _checked,int _row,bool knkv)//0=kn;1=kv
{
    if (!knkv)//kn
    {
        if (_checked)
        {
            stepsADD[_row].knkv|=1<<0;//устанавливаем нулевой бит (КН) в 1
            par_kn[_row]->setIcon(QIcon("./Resources/k1On_m.png"));
        }
        else 
        {
            stepsADD[_row].knkv&=~(1<<0);//устанавливаем нулевой бит (КН) в 0
            par_kn[_row]->setIcon(QIcon("./Resources/k1Off_m.png"));
        }
    }
    else//kv
    {
        if (_checked)
        {
            stepsADD[_row].knkv|=1<<2;//устанавливаем второй бит (КВ) в 1
            par_kv[_row]->setIcon(QIcon("./Resources/k2On_m.png"));
        }
        else
        {
            stepsADD[_row].knkv&=~(1<<2);//устанавливаем второй бит (КВ) в 0
            par_kv[_row]->setIcon(QIcon("./Resources/k2Off_m.png"));
        }
    }
    //_checked?(secondary_delta[_row].event=knkv?14:11):10;
    toDebug(getDeltaString(true));//data
}

void AdditionalButtons::timeChange(int _delta,int _row)//изменение дельты на основном шаге
{
    par_time[_row]->setTime(par_time[_row]->rawtime.addSecs(_delta*60+monitor::getTimeDifference()*60));//перевод из часового пояса терминала в часовой пояс монитора (секунды)
    stepsADD[_row].delta=_delta;
    toDebug(getDeltaString(true));//data
}

void AdditionalButtons::updateSecondaryDelta(QTime _time,int _row)//изменение дельты на дополнительном шаге
{
    //par_time[_row]->setTime(par_time[_row]->rawtime.addSecs(_delta*60));
    stepsADD[_row].delta=_time.hour()*60+_time.minute()-monitor::getTimeDifference();//перевод из часового пояса монитора в часовой пояс терминала (минуты)
    toDebug(getDeltaString(true));//data
}

/*int AdditionalButtons::getVoiceDeltaCommand(void)
{
    int voice_delta=0x0E000;
    int delta_tmp[4];
    delta_tmp[0]=delta_tmp[1]=delta_tmp[2]=delta_tmp[3]=0;
    for (int i=0;i<4;i++)
    for(int j=0;j<16;j++)
    {
        if (parent->delta[i].value==DeltaArr[j])
        {
            delta_tmp[i]=j;
            break;
        }
    }
    voice_delta|=(delta_tmp[0]<<16);
    voice_delta|=(delta_tmp[1]<<8);
    voice_delta|=(delta_tmp[2]<<4);
    voice_delta|=(delta_tmp[3]);
    return voice_delta;
}*/

QString AdditionalButtons::getDeltaString(bool data_mode)//
{
    QString tmp,tmpAdd,tmp_voice_delta;
    QTime tmp_time;
    _step temp_stepsADD[8];//массив шагов для сортировки и вывода в отображении шагов + дельт
    int old_delta=0,dl_num=0;

    tmp.clear();
    tmpAdd.clear();
    tmp_voice_delta.clear();

    tmp="DL=";
    memcpy(temp_stepsADD,stepsADD,sizeof(temp_stepsADD));
    qsort(temp_stepsADD,8,sizeof(_step),&AdditionalButtons::resort_stepsADD_toSend);//шаги сортируются подряд без учёта активности
    for (int i=0;i<8;i++)
    {
        if (temp_stepsADD[i].primary)
        {
            for (int j=0;j<16;j++)//ищется нужный индекс дельты
            {
                if (temp_stepsADD[i].delta==DeltaArr[j])
                {
                    tmp_voice_delta+=(i!=0?QString::number(j,16):QString::number(j,16)+"E");
                    break;
                }
            }

            tmp+=(temp_stepsADD[i].act?(QString::number(temp_stepsADD[i].delta)):"0")+((dl_num<3)?",":";AUD=3;");
            //"DL=5,15,-10,10;AUD=3;AUS=09:06,11;AUS=00:00,14;AUS=23:01,11;AUS=20:50,14;" 
            dl_num++;
        }
        else if (temp_stepsADD[i].act)  
        {
            tmp_time=QTime(0,0,0,0).addSecs(temp_stepsADD[i].delta*60);//часовой пояс терминала
            tmpAdd+="AUS="+tmp_time.toString("hh:mm")+","+QString::number(temp_stepsADD[i].knkv)+";";//.addSecs(monitor::getTimeDifference()*(-60))
        }
    }

    //QString vdelta=QString::number(temp_stepsADD[0].delta,16)+"E"+QString::number(temp_stepsADD[1].delta,16)+QString::number(temp_stepsADD[2].delta,16)+QString::number(temp_stepsADD[3].delta,16);
    return data_mode?tmp+tmpAdd:tmp_voice_delta;
}

int AdditionalButtons::sort_stepsADD(const void * p1,const void * p2)
{//дополнительная функция для qsort
    _step* struct1 = (_step*)p1;
    _step* struct2 = (_step*)p2;

    int diff_activity = struct2->act - struct1->act;
    if (diff_activity) return diff_activity;
    if (struct1->num < struct2->num) return -1;//сортируем по номеру шага
    else 
    if (struct1->num == struct2->num) return  0;//сортируем по локальному времени монитора
    else return  1;
}

int AdditionalButtons::resort_stepsADD_toSend(const void * p1,const void * p2)
{//дополнительная функция для qsort, основные шаги сначала, дополнительные в конце
    _step* struct1 = (_step*)p1;
    _step* struct2 = (_step*)p2;

    //int diff_activity = struct2->act - struct1->act;
    //if (diff_activity) return diff_activity;
    if (struct1->num < struct2->num) return -1;//сортируем по номеру шага
    else 
    if (struct1->num == struct2->num) return  0;//сортируем по локальному времени монитора
    else return  1;
}


void AdditionalButtons::toDebug(QString _text)
{
    //AdditionalButtons::debugLabel->setText(_text);
    qDebug() << _text.toUtf8();
    // old qDebug()<<_text.toAscii();
}
