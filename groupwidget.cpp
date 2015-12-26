#include "groupwidget.h"
#include <QtWidgets/QListWidgetItem>
#include <QSqlQuery>
#include <QPainter>
#include "additionalbuttons.h"
#include "basesettings.h"
#include "ipwindow.h"
#include "flowlayout.h"
#include <QtWidgets/QMessageBox>
#include "devicebutton.h"
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QFrame>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QDateTime>


GroupWidget::GroupWidget(QWidget *parent)
    : QWidget(parent)
{
    //BaseSettings *bs = BaseSettings::createInstance();
    ui.setupUi(this);
    //gI D = in_gI D;
    //mode = in_mode;
    //tabIndex = in_tabIndex;
    QFrame *frame = new QFrame();
    frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    frame->setFrameStyle(QFrame::Box|QFrame::Plain);
    FL = new FlowLayout();
    frame->setLayout(FL);

    scrollArea = new QScrollArea();
    scrollArea->setWidget(frame);
    scrollArea->setWidgetResizable(true);
    ui.vboxLayout->addWidget(scrollArea);

    QFrame *frame2 = new QFrame();
    frame2->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    frame2->setFrameStyle(QFrame::Box|QFrame::Plain);
    FL2 = new FlowLayout();
    frame2->setLayout(FL2);
    scrollArea2 = new QScrollArea();
    scrollArea2->setWidget(frame2);
    scrollArea2->setWidgetResizable(true);
    ui.vboxLayout->addWidget(scrollArea2);
    scrollArea2->hide();
    //scrollArea->setWidget(); 
    //ui.vboxLayout->addWidget(scrollArea);
    //tabText = in_tabText;
    
    //scrollArea->setLayout(FL);
    //ui.listWidget->setViewMode(QListView::IconMode);
    //ui.listWidget->setFlow(QListView::LeftToRight);
    //updateFromBD();
    //connect(ui.listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *) ),this,SLOT(onItemDblClick(QListWidgetItem *)));
    //connect(bs->ipW,SIGNAL(refreshList()),this,SLOT(updateFromBD()));
    /*QSqlQuery *tq = new QSqlQuery("select * from send_commands where (rec_id != 3) and (rec_id != 14) and (rec_id < 16)");
    if (tq->isActive()){
        while (tq->next()){
            ui.cbCommand->addItem(tq->value(2).toString(),tq->value(1).toInt());
        }
    }*/
    //autoTimer = new QTimer(NULL);
    //timeToTimer = new QTimer(NULL);
    //connect(autoTimer,SIGNAL(timeout()),this,SLOT(onAutoTimer()));
    //connect(timeToTimer,SIGNAL(timeout()),this,SLOT(onTimeToTimer()));
    //ui.labelTo->setText("");
    isAutomationActive = false;
    key_group=false;
    //key_circlecommand=false;

    list_index.clear();

    debug = new QFile();
    BaseSettings *bs = BaseSettings::createInstance();
    buttons_config=bs->settings->value("Options/buttons_config","0").toInt();
    pathlog = bs->settings->value("DirLogFile/path","./").toString();
    pathlog.replace(QString("\\"), QString("/"));
    bs->freeInst();
}

GroupWidget::~GroupWidget()
{
    //ui.listWidget->clear();
    //bs->freeInst();
    /*if (autoTimer->isActive()){
        autoTimer->stop();
    }
    if (timeToTimer->isActive())
    {
        timeToTimer->stop();
    }
    delete autoTimer;
    delete timeToTimer;*/
    if(!list_index.isEmpty())
    {
        list_index.clear(); 
    }
    if (debug) delete debug;
}

void GroupWidget::showArea2()
{
    scrollArea2->show();
}

void GroupWidget::hideArea2()
{
    scrollArea2->hide();
}

void GroupWidget::logparser(QString str)
{
    QString temp;
    debug->setFileName(pathlog+tr("Поставлена в базу_")+QDateTime::currentDateTime().toString("yyyy.MM.dd")+".log");

    temp=temp+QDateTime::currentDateTime().toString("hh:mm:ss        ")+str;
    
    if (debug->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(debug);
        out.setCodec("UTF-8");
        out << temp << "\n";
        out.flush();
        debug->flush();
        debug->close();
    }
}

void GroupWidget::updateFromBD(){
}

void GroupWidget::on_pushButton_clicked()
{
    //off all
    reAskDisp(tr(" выключить все устройства в группе?"),8);
}

void GroupWidget::on_pushButton_KH_clicked()
{
    //1	0
    reAskDisp(tr(" включить КН всех устройств в группе?"),9);
}

void GroupWidget::on_pushButton_KB_clicked()
{
    //0	1
    reAskDisp(tr(" включить КВ всех устройств в группе?"),10);
}

void GroupWidget::on_pushButton_4_clicked()
{
    //on all
    reAskDisp(tr(" включить оба контактора всех устройств в группе?"),11);
}

void GroupWidget::on_pushButton_5_clicked()
{
    //ask state
    reAskDisp(tr(" запросить состояние всей группы?"),5);
}

void GroupWidget::on_pushButton_6_clicked()
{
    //to avto
    reAskDisp(tr(" перевести всю группу в автоном?"),12);
}

void GroupWidget::on_pbGroupDelta_clicked()
{
    //delta
    generateDeltaCommands();
}

void GroupWidget::setParent(monitor* in_ptr)
{
    parent=(monitor*) in_ptr;
}

void GroupWidget::reAskDisp(QString txtCommand, int intCommand)
{
    QString str;
    str.append(qApp->tr("Вы уверены, что хотите\n"));
    str.append(txtCommand);
    int ret = QMessageBox::question(0, qApp->tr("Подтверждение"),
        str, QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel);
    if (ret != QMessageBox::Ok) return;
    int deviceID;
    bool data_mode;
    QSqlQuery *tq = NULL;
//  QSqlQuery *tq1 = NULL;
    switch(mode)
    {
        case MODE_GG:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device where group_g = "+QString::number(gID,10));
            break;
        case MODE_GA:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device where group_a = "+QString::number(gID,10));
            break;
        case MODE_STATE:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device where is_error = "+QString::number(gID,10));
            break;
        case MODE_ALL:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device");
            break;
        default:break;
    }
//  tq1 = new QSqlQuery();

    str.clear();
    while (tq->next())//для каждого прибора в группе
    {
        deviceID=tq->value(0).toInt();
        data_mode=tq->value(1).toBool();
        int delta_time,difTime,normalDiff;
        QDateTime utcTime,currentTime;
        QString time_str;
        switch(intCommand)
        {
            case 5:str=(data_mode&&CHECK_BIT(buttons_config,1))?"ST=?;":"5";break;
            case 8:str=(data_mode&&CHECK_BIT(buttons_config,3))?"UC=8;":"8";break;
            case 9:str=(data_mode&&CHECK_BIT(buttons_config,3))?"UC=9;":"9";break;
            case 10:str=(data_mode&&CHECK_BIT(buttons_config,3))?"UC=10;":"A";break;
            case 11:str=(data_mode&&CHECK_BIT(buttons_config,3))?"UC=11;":"B";break;
            case 12:normalDiff =parent->mainBs->settings->value("System/normal_UTC_diff",3).toInt();
                    currentTime = QDateTime::currentDateTime();
                    utcTime = currentTime.toUTC();
                    utcTime.setTimeSpec(Qt::LocalTime);
                    difTime = currentTime.toTime_t() - utcTime.toTime_t();
                    delta_time=difTime-normalDiff*60*60;
                    delta_time=delta_time/3600;
                    currentTime.setTime_t(currentTime.toTime_t() - delta_time*60*60);
                    time_str.clear();
                    time_str.append("TM=");
                    time_str.append(currentTime.toString("yy/MM/dd,hh:mm:ss"));
                    time_str.append(";UC=12;");
                    str=(data_mode&&CHECK_BIT(buttons_config,5))?time_str:"C";break;
            default:str=(data_mode&&CHECK_BIT(buttons_config,1))?"ST=?;":"5";break;
        }
        emit putCommand(deviceID,str,intCommand+data_mode*50);
    }
    if (tq) delete tq;
    //if (tq1) delete tq1;
}

void GroupWidget::pic_State(int pos,DeviceButton * cur_obj)
{
    QString str;
    bool key=false;
    int buf[3];
    bool conf[9];

    QSqlQuery *tq=new QSqlQuery();
    QSqlQuery *tqDeviceState = new QSqlQuery("select k1_present, k2_present, door_present"		
            " from device where rec_id = "
            +QString::number(cur_obj->master->rec_id,10)+
            " order by rec_id desc limit 1");
    if (tqDeviceState->isActive())
    {
        //good
        tqDeviceState->next();
        buf[0] = tqDeviceState->value(0).toInt();
        buf[1] = tqDeviceState->value(1).toInt();
        buf[2] = tqDeviceState->value(2).toInt();
    }
    QSqlQuery *tqDeviceConfig = new QSqlQuery("select is_present from shuno_config where device_id = "
        +QString::number(cur_obj->master->rec_id,10)+" order by rec_id");
    if (tqDeviceConfig->isActive())
    {
        //good
        for(int i=0;i<9;i++)
        {
            tqDeviceConfig->next();
            conf[i] = (bool)tqDeviceConfig->value(0).toInt();
        }
    }
    
    cur_obj->master->updateDevice();

    if(cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
    {
        if(buf[0]==1 && buf[1]==1)
            key=true;
    }
    else if(!cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
    {
        if(buf[0]==0 && buf[1]==1)
            key=true;
    }
    else if(cur_obj->master->deviceDescr.k1_present && !cur_obj->master->deviceDescr.k2_present)
    {
        if(buf[0]==1 && buf[1]==0)
            key=true;
    }

    switch(pos)
    {
    case 8:
        str.clear();
        
        if(key)
        {
            if(cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
            {
                str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,4,4,";
                for(int i=0;i<9;i++)
                {
                    if(conf[i])
                        str=str+"4,";
                    else
                        str=str+"1,";
                }
                str=str+"2)";
            }
            else if(!cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
            {
                str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,1,4,1,1,1,";
                for(int i=3;i<9;i++)
                {
                    if(conf[i])
                        str=str+"4,";
                    else
                        str=str+"1,";
                }
                str=str+"2)";
            }
            else if(cur_obj->master->deviceDescr.k1_present && !cur_obj->master->deviceDescr.k2_present)
            {
                str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,4,1,";
                for(int i=0;i<3;i++)
                {
                    if(conf[i])
                        str=str+"4,";
                    else
                        str=str+"1,";
                }
                str=str+"1,1,1,1,1,1,2)";
            }
            tq->exec("insert INTO dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
            //tq->exec("replace dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        cur_obj->master->deviceDescr.is_viewed=false;
        cur_obj->master->updateDevice();
        cur_obj->master->updateState();
        cur_obj->master->updateMainIcon();
        cur_obj->master->updateImages();
        }
        break;
    case 9:
        str.clear();
        if(key)
        {
        if(cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,2,4,";
            for(int i=0;i<3;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            for(int i=3;i<9;i++)
            {
                if(conf[i])
                    str=str+"4,";
                else
                    str=str+"1,";
            }
            str=str+"2)";
        }
        else if(!cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,1,4,1,1,1,";
            for(int i=3;i<9;i++)
            {
                if(conf[i])
                    str=str+"4,";
                else
                    str=str+"1,";
            }
            str=str+"2)";
        }
        else if(cur_obj->master->deviceDescr.k1_present && !cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,2,1,";
            for(int i=0;i<3;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            str=str+"1,1,1,1,1,1,2)";
        }
        //tq->exec("insert INTO dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        tq->exec("replace dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        cur_obj->master->deviceDescr.is_viewed=false;
        cur_obj->master->updateDevice();
        cur_obj->master->updateState();
        cur_obj->master->updateMainIcon();
        cur_obj->master->updateImages();
        }
        break;
    case 10:
        str.clear();
        if(key)
        {
        if(cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,4,2,";
            for(int i=0;i<3;i++)
            {
                if(conf[i])
                    str=str+"4,";
                else
                    str=str+"1,";
            }
            for(int i=3;i<9;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            str=str+"2)";
        }
        else if(!cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,1,2,1,1,1,";
            for(int i=3;i<9;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            str=str+"2)";
        }
        else if(cur_obj->master->deviceDescr.k1_present && !cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,4,1,";
            for(int i=0;i<3;i++)
            {
                if(conf[i])
                    str=str+"4,";
                else
                    str=str+"1,";
            }
            str=str+"1,1,1,1,1,1,2)";
        }
        //tq->exec("insert INTO dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        tq->exec("replace dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        cur_obj->master->deviceDescr.is_viewed=false;
        cur_obj->master->updateDevice();
        cur_obj->master->updateState();
        cur_obj->master->updateMainIcon();
        cur_obj->master->updateImages();
        }

        break;
    case 11:
        str.clear();
        if(key)
        {
        if(cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,2,2,";
            for(int i=0;i<9;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            str=str+"2)";
        }
        else if(!cur_obj->master->deviceDescr.k1_present && cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,1,2,1,1,1,";
            for(int i=3;i<9;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            str=str+"2)";
        }
        else if(cur_obj->master->deviceDescr.k1_present && !cur_obj->master->deviceDescr.k2_present)
        {
            str=str+"values ("+QString::number(cur_obj->master->rec_id,10)+",NOW(),2,2,2,2,1,";
            for(int i=3;i<6;i++)
            {
                if(conf[i])
                    str=str+"2,";
                else
                    str=str+"1,";
            }
            str=str+"1,1,1,1,1,1,2)";
        }
        //tq->exec("insert INTO dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        tq->exec("replace dev_state (device_id,time_stamp,a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state) "+str);
        cur_obj->master->deviceDescr.is_viewed=false;
        cur_obj->master->updateDevice();
        cur_obj->master->updateState();
        cur_obj->master->updateMainIcon();
        cur_obj->master->updateImages();
        }
        
        break;
    }
    delete tqDeviceState;
    delete tqDeviceConfig;
    delete tq;
}
//void GroupWidget::on_pbDelQueryCommand_clicked()
//{
    //удаление (помечение как удаленных)
//}

void GroupWidget::on_pbClearAhtung_clicked()
{
    QSqlQuery *tq;
    switch(mode){
        case MODE_GG:
            tq = new QSqlQuery("update device set is_viewed=1 where group_g = "+QString::number(gID,10));
            break;
        case MODE_GA:
            tq = new QSqlQuery("update device set is_viewed=1 where group_a = "+QString::number(gID,10));
            break;
        case MODE_STATE:
            tq = new QSqlQuery("update device set is_viewed=1 where is_error = "+QString::number(gID,10));
            break;
        case MODE_ALL:
            tq = new QSqlQuery("update device set is_viewed=1");
            break;
            /*
            switch(gID)	{
                case 0://Ручной - ОК
                    tq = new QSqlQuery("update device set is_viewed=1 where is_avto=0 and is_error=0");
                    break;
                case 1://Авто - ОК
                    tq = new QSqlQuery("update device set is_viewed=1 where is_avto=1 and is_error=0");
                    break;
                case 2://Ручной - Авария
                    tq = new QSqlQuery("update device set is_viewed=1 where is_avto=0 and is_error=1");
                    break;
                case 3://Авто - Авария
                    tq = new QSqlQuery("update device set is_viewed=1 where is_avto=1 and is_error=1");
                    break;
                default:
                    break;
            }*/
            break;
            case MODE_COUNT:
                //tq = new QSqlQuery("update device set is_viewed=1 where door_present = "+QString::number(gID,10));
                switch(gID)
                {
                case 0://Дверь - ОК
                    tq = new QSqlQuery("update device set is_viewed=1 where door_present=1");
                    break;
                case 1://Дверь - NO
                    tq = new QSqlQuery("update device set is_viewed=1 where door_present=0");
                    break;
                }
            break;
    }
    tq->exec();
    delete tq;
    //updateFromBD();
    emit clearAhtung(mode, gID);
}

void GroupWidget::AddDevice(DeviceButton *in_b)
{
    FL->addWidget(in_b);
    list_index.append(in_b);
}

void GroupWidget::DelDevice(DeviceButton *in_b)
{
    FL->removeWidget(in_b);
    if(!list_index.isEmpty())
    {
        for(int i=0;i<list_index.count();i++)
        {
            if(list_index.at(i)->master->rec_id==in_b->master->rec_id)
            {
                list_index.removeAt(i);
                break;
            }
        }
    }
}

void GroupWidget::setMonitorMode(MonitorShowMode in_mode){
    mode = in_mode;
}

void GroupWidget::setGID(int in_gID){
    gID = in_gID;
}

void GroupWidget::setHideWgt()
{
    ui.groupBox->hide();
}

void GroupWidget::setButED()
{
    if(!key_group)
    {
        ui.pushButton->setDisabled(true);
        ui.pushButton_KH->setDisabled(true);
        ui.pushButton_KB->setDisabled(true);
        ui.pushButton_4->setDisabled(true);
        ui.pushButton_5->setDisabled(true);
        ui.pushButton_6->setDisabled(true);
    }
    else
    {
        ui.pushButton->setDisabled(false);
        ui.pushButton_KH->setDisabled(false);
        ui.pushButton_KB->setDisabled(false);
        ui.pushButton_4->setDisabled(false);
        ui.pushButton_5->setDisabled(false);
        ui.pushButton_6->setDisabled(false);
    }
}

void GroupWidget::generateDeltaCommands()
{
    DevList * deviceListToSend=new DevList();
    QString str;
    bool data_present=false;
    QSqlQuery *tq = NULL;
    switch(mode)
    {
        case MODE_GG:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device where group_g = "+QString::number(gID,10));
            break;
        case MODE_GA:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device where group_a = "+QString::number(gID,10));
            break;
        case MODE_STATE:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device where is_error = "+QString::number(gID,10));
            break;
        case MODE_ALL:
            tq = new QSqlQuery("select rec_id, data_mode, phone from device");
            break;
        default:break;
    }
    str.clear();
    while (tq->next())//для каждого прибора в группе
    {
        IPObject *tO = new IPObject(parent,tq->value(0).toInt());
        deviceListToSend->append(tO);
        //deviceID=tq->value(0).toInt();
        data_present=tq->value(1).toBool()?true:data_present;
        //deviceListToSend->append(IPObject xzxz);
        //emit putCommand(deviceID,str,intCommand+data_mode*50);
    }
    if (tq) delete tq;
    if (data_present)
    {
        parent->moreButtons_group->groupShow(deviceListToSend);
    }
    else 
    {
        QMessageBox warning_voice;
        warning_voice.setText(tr("В группе не обнаружено приборов, которые поддерживают установку дополнительных шагов и задание произвольных смещений основных шагов ПАР. Рекомендуется выяснить возможность их перенастройки в <a href=\"http://ow.od.ua/?page_id=93\">сервисном центре производителя</a>. До этого времени единственным выходом из сложившейся ситуации является отправка смещений шагов ПАР на каждый прибор индивидуально."));
        warning_voice.setIcon(QMessageBox::Critical);
        warning_voice.exec();
    }
}
//    str.append(qApp->tr("Вы уверены, что хотите\n"));
//    str.append(txtCommand);
//    int ret = QMessageBox::question(0, qApp->tr("Подтверждение"),
//        str, QMessageBox::Ok | QMessageBox::Cancel,
//        QMessageBox::Cancel);
//    if (ret != QMessageBox::Ok) return;

