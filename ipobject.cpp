#include "ipobject.h"
#include <QSqlQuery>
#include <QPainter>
#include "devicebutton.h"
#include "basesettings.h"
#include "ipwindow.h"

IPObject::IPObject(monitor *parent, int in_index)
    : QObject(parent)
{
    rec_id = in_index;
    monitorIcon = new QImage(60,60,QImage::Format_ARGB32);
    details = new QImage(530,330,QImage::Format_ARGB32);
    devButton = new DeviceButton(NULL);
    devButton->setIndex(this);
    devButton->setDetails(details);
    //connect(devButton,SIGNAL(deviceClicked(IPObject *)),this,SLOT(getDeviceClick(IPObject *)));
    connect(devButton,SIGNAL(deviceClicked(IPObject *)),parent,SLOT(get_Device_Click(IPObject *)));//перенесено в DeviceButton

    key_anim=false;
    deviceDescr.counter_pass = -1;//во время работы монитора counter_pass может меняться, при перезагрузке - по умолчанию -1
    deviceDescr.counter_toSend = false;//во время работы монитора counter_present может меняться, при перезагрузке - по умолчанию false
    bpf_state=false;
    bpf_ph_kn=-1;
    bpf_ph_kv=-1;
    mess_state=-1;
    setCol();
    //refreshData();
    //updateDevice();
    loadDevice();//первичная загрузка параметров прибора из базы
    updateConfig();
    updateState();
    updateImages();
}

IPObject::~IPObject()
{
    delete monitorIcon;
    delete details;
    delete devButton;
}

void IPObject::loadDevice(){
    QSqlQuery *tqDevice = new QSqlQuery("select "
        "name, "                    //0
        "phone, "                   //1
        "adress, "                  //2
        "dev_hard_number, "         //3
        "group_g, "                 //4
        "group_a, "                 //5
        "avtonom_present, "         //6
        //"counter_present, "           //7
        "counter_precision, "       //7
        "b_present, "               //8
        "c_present, "               //9 
        "k1_present, "              //10
        "k2_present, "              //11
        "door_present, "            //12
        "is_error, "                //13
        "is_avto, "                 //14
        "notes, "                   //15
        "x, "                       //16
        "y, "                       //17
        "counter_1_text, "          //18
        "counter_2_text, "          //19
        "counter_1_low_int, "       //20
        "counter_2_low_int, "       //21
        "counter_1_hi_int, "        //22
        "counter_2_hi_int, "        //23
        "configured, "              //24
        "is_viewed, "               //25
        "is_crc, "                  //26
        "is_root, "                 //27
        "position, "                //28
        "delta_1, "                 //29
        "delta_2, "                 //30
        "delta_3, "                 //31
        "delta_4, "                 //32
        "delta_5, "                 //33
        "delta_6, "                 //34
        "delta_7, "                 //35
        "delta_8, "                 //36
        "delta_9, "                 //37
        "delta_10, "                //38
        "delta_11, "                //39
        "delta_12, "                //40
        "delta_13, "                //41
        "delta_14, "                //42
        "delta_15, "                //43
        "delta_16, "                //44
        "counter_1_date, "          //45
        "counter_2_date, "          //46
        "data_mode, "               //47
        "counter_number, "          //48
        "counter_type "             //49
        " from device where rec_id = "
        +QString::number(rec_id,10));
    if (tqDevice->isActive()){
        //good
        tqDevice->next();
        deviceDescr.name = tqDevice->value(0).toString();
        deviceDescr.phone = tqDevice->value(1).toString();
        deviceDescr.adress = tqDevice->value(2).toString();
        deviceDescr.dev_hard_number = tqDevice->value(3).toInt();
        deviceDescr.group_g = tqDevice->value(4).toInt();
        deviceDescr.group_a = tqDevice->value(5).toInt();
        deviceDescr.avtonom_present = tqDevice->value(6).toInt();///!!! был toBool(), до того как стали хранить в нём состояние контакторов для дополнительных шагов
        //deviceDescr.counter_p resent = tqDevice->value(7).toBool();//устанавливается при начале работы монитора
        deviceDescr.counter_precision = tqDevice->value(7).toInt();
        deviceDescr.b_present = tqDevice->value(8).toBool();
        deviceDescr.c_present = tqDevice->value(9).toBool();
        deviceDescr.k1_present = tqDevice->value(10).toBool();
        deviceDescr.k2_present = tqDevice->value(11).toBool();
        deviceDescr.door_present = tqDevice->value(12).toBool();
        deviceDescr.is_error = tqDevice->value(13).toBool();
        deviceDescr.is_avto = tqDevice->value(14).toInt();
        deviceDescr.notes = tqDevice->value(15).toString();
        deviceDescr.x = tqDevice->value(16).toDouble();
        deviceDescr.y = tqDevice->value(17).toDouble();
        deviceDescr.counter_1_text = tqDevice->value(18).toString();
        deviceDescr.counter_2_text = tqDevice->value(19).toString();
        deviceDescr.counter_1_low_int = tqDevice->value(20).toInt();
        deviceDescr.counter_2_low_int = tqDevice->value(21).toInt();
        deviceDescr.counter_1_hi_int = tqDevice->value(22).toInt();
        deviceDescr.counter_2_hi_int = tqDevice->value(23).toInt();
        deviceDescr.is_configured = tqDevice->value(24).toBool();
        deviceDescr.is_viewed = tqDevice->value(25).toBool();
        deviceDescr.is_crc = tqDevice->value(26).toBool();
        deviceDescr.is_root = tqDevice->value(27).toBool();
        deviceDescr.position = tqDevice->value(28).toInt();
        deviceDescr.delta_1 = tqDevice->value(29).toInt();
        deviceDescr.delta_2 = tqDevice->value(30).toInt();
        deviceDescr.delta_3 = tqDevice->value(31).toInt();
        deviceDescr.delta_4 = tqDevice->value(32).toInt();
        deviceDescr.delta_5 = tqDevice->value(33).toInt();
        deviceDescr.delta_6 = tqDevice->value(34).toInt();
        deviceDescr.delta_7 = tqDevice->value(35).toInt();
        deviceDescr.delta_8 = tqDevice->value(36).toInt();
        deviceDescr.delta_9 = tqDevice->value(37).toInt();
        deviceDescr.delta_10 = tqDevice->value(38).toInt();
        deviceDescr.delta_11 = tqDevice->value(39).toInt();
        deviceDescr.delta_12 = tqDevice->value(40).toInt();
        deviceDescr.delta_13 = tqDevice->value(41).toInt();
        deviceDescr.delta_14 = tqDevice->value(42).toInt();
        deviceDescr.delta_15 = tqDevice->value(43).toInt();
        deviceDescr.delta_16 = tqDevice->value(44).toInt();
        deviceDescr.counter_1_date = tqDevice->value(45).toString();
        deviceDescr.counter_2_date = tqDevice->value(46).toString();
        deviceDescr.data_mode = tqDevice->value(47).toBool();
        deviceDescr.counter_number = tqDevice->value(48).toInt();
        deviceDescr.counter_type = tqDevice->value(49).toInt();
    }
/*  tqDevice->exec("select data_mode from device where rec_id = "+QString::number(rec_id,10));
    if (tqDevice->isActive())
    {
        tqDevice->next();
        deviceDescr.data_mode = tqDevice->value(0).toBool();
    }

    tqDevice->exec("select counter_number, counter_type from device where rec_id = "+QString::number(rec_id,10));
    if (tqDevice->isActive())
    {
        tqDevice->next();
        deviceDescr.counter_number = tqDevice->value(0).toInt();
        deviceDescr.counter_type = tqDevice->value(1).toInt();
    }*/

    //логичнее будет не снимать непросмотренность при запуске монитора
    /*BaseSettings *bs = BaseSettings::createInstance();
    if (bs->ipW->isVisible()){
        deviceDescr.is_viewed = TRUE;
        QSqlQuery tq;
        tq.exec("update device set is_viewed=1 where rec_id ="+QString::number(rec_id,10));
    }
    bs->freeInst();*/
    //////////////////////////////////////////////////////////////////////////
    delete tqDevice;
}

void IPObject::updateDevice(){
    QSqlQuery *tqDevice = new QSqlQuery("select "
        "name, "                    //0
        "phone, "                   //1
        "adress, "                  //2
        "dev_hard_number, "         //3
        "group_g, "                 //4
        "group_a, "                 //5
        "avtonom_present, "         //6
        //"counter_present, "           //7
        //"counter_precision, "         //8
        "b_present, "               //9
        "c_present, "               //10
        "k1_present, "              //11
        "k2_present, "              //12
        "door_present, "            //13
        "is_error, "                //14
        "is_avto, "                 //15
        "notes, "                   //16
        "x, "                       //17
        "y, "                       //18
        "counter_1_text, "          //19
        "counter_2_text, "          //20
        "counter_1_low_int, "       //21
        "counter_2_low_int, "       //22
        "counter_1_hi_int, "        //23
        "counter_2_hi_int, "        //24
        "configured, "              //25
        "is_viewed, "               //26
        "is_crc, "                  //27
        "is_root, "                 //28
        "position, "                //29
        "delta_1, "                 //30
        "delta_2, "                 //31
        "delta_3, "                 //32
        "delta_4, "                 //33
        "delta_5, "                 //34
        "delta_6, "                 //35
        "delta_7, "                 //36
        "delta_8, "                 //37
        "delta_9, "                 //38
        "delta_10, "                //39
        "delta_11, "                //40
        "delta_12, "                //41
        "delta_13, "                //42
        "delta_14, "                //43
        "delta_15, "                //44
        "delta_16, "                //45
        "counter_1_date, "          //46
        "counter_2_date, "          //47
        "data_mode "                //48
        //"counter_number, "            //49
        //"counter_type "               //50
        " from device where rec_id = "
        +QString::number(rec_id,10));
    if (tqDevice->isActive()){
        //good
        tqDevice->next();
        deviceDescr.name = tqDevice->value(0).toString();
        deviceDescr.phone = tqDevice->value(1).toString();
        deviceDescr.adress = tqDevice->value(2).toString();
        deviceDescr.dev_hard_number = tqDevice->value(3).toInt();
        deviceDescr.group_g = tqDevice->value(4).toInt();
        deviceDescr.group_a = tqDevice->value(5).toInt();
        deviceDescr.avtonom_present = tqDevice->value(6).toInt();///!!! был toBool(), до того как стали хранить в нём состояние контакторов для дополнительных шагов
        //deviceDescr.counter_p resent = tqDevice->value(7).toBool();//устанавливается при начале работы монитора
        //deviceDescr.counter_precision = tqDevice->value(8).toInt();
        deviceDescr.b_present = tqDevice->value(7).toBool();
        deviceDescr.c_present = tqDevice->value(8).toBool();
        deviceDescr.k1_present = tqDevice->value(9).toBool();
        deviceDescr.k2_present = tqDevice->value(10).toBool();
        deviceDescr.door_present = tqDevice->value(11).toBool();
        deviceDescr.is_error = tqDevice->value(12).toBool();
        deviceDescr.is_avto = tqDevice->value(13).toInt();
        deviceDescr.notes = tqDevice->value(14).toString();
        deviceDescr.x = tqDevice->value(15).toDouble();
        deviceDescr.y = tqDevice->value(16).toDouble();
        deviceDescr.counter_1_text = tqDevice->value(17).toString();
        deviceDescr.counter_2_text = tqDevice->value(18).toString();
        deviceDescr.counter_1_low_int = tqDevice->value(19).toInt();
        deviceDescr.counter_2_low_int = tqDevice->value(20).toInt();
        deviceDescr.counter_1_hi_int = tqDevice->value(21).toInt();
        deviceDescr.counter_2_hi_int = tqDevice->value(22).toInt();
        deviceDescr.is_configured = tqDevice->value(23).toBool();
        deviceDescr.is_viewed = tqDevice->value(24).toBool();
        deviceDescr.is_crc = tqDevice->value(25).toBool();
        deviceDescr.is_root = tqDevice->value(26).toBool();
        deviceDescr.position = tqDevice->value(27).toInt();
        deviceDescr.delta_1 = tqDevice->value(28).toInt();
        deviceDescr.delta_2 = tqDevice->value(29).toInt();
        deviceDescr.delta_3 = tqDevice->value(30).toInt();
        deviceDescr.delta_4 = tqDevice->value(31).toInt();
        deviceDescr.delta_5 = tqDevice->value(32).toInt();
        deviceDescr.delta_6 = tqDevice->value(33).toInt();
        deviceDescr.delta_7 = tqDevice->value(34).toInt();
        deviceDescr.delta_8 = tqDevice->value(35).toInt();
        deviceDescr.delta_9 = tqDevice->value(36).toInt();
        deviceDescr.delta_10 = tqDevice->value(37).toInt();
        deviceDescr.delta_11 = tqDevice->value(38).toInt();
        deviceDescr.delta_12 = tqDevice->value(39).toInt();
        deviceDescr.delta_13 = tqDevice->value(40).toInt();
        deviceDescr.delta_14 = tqDevice->value(41).toInt();
        deviceDescr.delta_15 = tqDevice->value(42).toInt();
        deviceDescr.delta_16 = tqDevice->value(43).toInt();
        deviceDescr.counter_1_date = tqDevice->value(44).toString();
        deviceDescr.counter_2_date = tqDevice->value(45).toString();
        deviceDescr.data_mode = tqDevice->value(46).toBool();
        //deviceDescr.counter_number = tqDevice->value(49).toInt();
        //deviceDescr.counter_type = tqDevice->value(50).toInt();
    }
/*  tqDevice->exec("select data_mode from device where rec_id = "+QString::number(rec_id,10));
    if (tqDevice->isActive())
    {
        tqDevice->next();
        deviceDescr.data_mode = tqDevice->value(0).toBool();
    }

    tqDevice->exec("select counter_number, counter_type from device where rec_id = "+QString::number(rec_id,10));
    if (tqDevice->isActive())
    {
        tqDevice->next();
        deviceDescr.counter_number = tqDevice->value(0).toInt();
        deviceDescr.counter_type = tqDevice->value(1).toInt();
    }*/

/*  BaseSettings *bs = BaseSettings::createInstance();
    if (bs->ipW->isVisible()){
        deviceDescr.is_viewed = TRUE;
        QSqlQuery tq;
        tq.exec("update device set is_viewed=1 where rec_id ="+QString::number(rec_id,10));
    }
    bs->freeInst();*/
    //////////////////////////////////////////////////////////////////////////
    delete tqDevice;
}

void IPObject::updateConfig(){
    QSqlQuery *tqDeviceConfig = new QSqlQuery("select "
        "ckeck_point_number, "  //0
        "k_number, "            //1
        "phase_number, "        //2
        "human_name, "          //3
        "type, "                //4
        "is_present, "          //5
        "is_error, "            //6
        "last_state "           //7
        " from shuno_config where device_id = "
        +QString::number(rec_id,10)+
        " order by rec_id");
    if (tqDeviceConfig->isActive()){
        //good
        for(int i=0;i<9;i++){
            tqDeviceConfig->next();
            deviceConfig[i].ckeck_point_number = tqDeviceConfig->value(0).toInt();
            deviceConfig[i].k_number = tqDeviceConfig->value(1).toInt();
            deviceConfig[i].phase_number = tqDeviceConfig->value(2).toInt();
            deviceConfig[i].human_name = tqDeviceConfig->value(3).toString();
            deviceConfig[i].type = tqDeviceConfig->value(4).toInt();
            deviceConfig[i].is_present = tqDeviceConfig->value(5).toBool();
            deviceConfig[i].is_error = tqDeviceConfig->value(6).toBool();
            deviceConfig[i].last_state = tqDeviceConfig->value(7).toInt();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    delete tqDeviceConfig;
}

bool IPObject::bitstate(int inData,int pos)
{
    return (((inData) & (1<<(pos)))>0);
}

void IPObject::updateState()
{
    int oldRawData,rawData=0,_Configured=255;
    //bool bit;
    bool C,X13,X14,X15,YH,YB,Z3,Z2;
    bool key=false;

    bpf_ph_kv=-1;
    bpf_ph_kn=-1;

    QSqlQuery *recv_state_rawData = new QSqlQuery("select raw_data from recv_state where device_id="+QString::number(rec_id,10)+"  ORDER BY rec_id DESC LIMIT 0,1");
    if (recv_state_rawData -> isActive())
    {
        recv_state_rawData -> next();
        oldRawData=recv_state_rawData -> value(0).toInt();
        if (oldRawData<16)
        {
            rawData|=oldRawData&4 ? (1<<18) : 0;//преобразовываем 1байт квитанцию в 5байт:      автоном
            rawData|=               (1<<15)+(1<<14)+(1<<13);//                                              фазы A B и C
            rawData|=oldRawData&2 ? (1<<17)+(1<<9)+(1<<8)+(1<<7)+(1<<6)+(1<<5)+(1<<4)+(1<<2) : 0;//если включён КВ: биты 17 - КВ(заданное); 9,8,7,6,5,4 - предохранители 4,5,6,7,8,9; 2 - Uкв(измеренное)
            rawData|=oldRawData&1 ? (1<<16)+(1<<12)+(1<<11)+(1<<10)+(1<<3) : 0;//если включён КН: биты 16 - КН(заданное); 12,11,10 - предохранители 1,2,3; 3 - Uкн(измеренное)
        }
        else rawData=oldRawData;
    }

    QSqlQuery *device_Configured = new QSqlQuery("select k1_present,k2_present,configured from device where rec_id="+QString::number(rec_id,10));
    if (device_Configured -> isActive())
    {
        device_Configured -> next();
        Z3=device_Configured -> value(0).toInt();//KH
        Z2=device_Configured -> value(1).toInt();//KB
        _Configured=device_Configured -> value(2).toInt();
        YH=bitstate(_Configured,0);//YH
        YB=bitstate(_Configured,1);//YB
        C=bitstate(_Configured,2);
    }

    X15=(bitstate(rawData,15)?true:false);//A
    X14=(bitstate(rawData,14)?true:false);//B
    X13=(bitstate(rawData,13)?true:false);//C
    bpf_ph_kn=(!C)*Z3*(!YH)*5 + (!C)*YH*6 + (!C)*(!Z3)*(!YH)*1 + C*(!Z3)*(!YH)*(!X15)*(!X14)*(!X13)*1 + C*(!Z3)*(!YH)*(!X15)*(!X14)*X13*1 + C*(!Z3)*(!YH)*(!X15)*X14*1 + C*(!Z3)*(!YH)*X15*1 + C*Z3*(!YH)*(!X15)*(!X14)*(!X13)*5 + C*Z3*(!YH)*(!X15)*(!X14)*X13*5 + C*Z3*(!YH)*(!X15)*X14*5 + C*Z3*(!YH)*X15*5 + C*YH*(!X15)*(!X14)*(!X13)*5 + C*YH*(!X15)*(!X14)*X13*4 + C*YH*(!X15)*X14*3 + C*YH*X15*2;
    bpf_ph_kv=(!C)*Z2*(!YB)*5 + (!C)*YB*6 + (!C)*(!Z2)*(!YB)*1 + C*(!Z2)*(!YB)*(!X15)*(!X14)*(!X13)*1 + C*(!Z2)*(!YB)*(!X15)*(!X14)*X13*1 + C*(!Z2)*(!YB)*(!X15)*X14*1 + C*(!Z2)*(!YB)*X15*1 + C*Z2*(!YB)*(!X15)*(!X14)*(!X13)*5 + C*Z2*(!YB)*(!X15)*(!X14)*X13*5 + C*Z2*(!YB)*(!X15)*X14*5 + C*Z2*(!YB)*X15*5 + C*YB*(!X15)*(!X14)*(!X13)*5 + C*YB*(!X15)*(!X14)*X13*4 + C*YB*(!X15)*X14*3 + C*YB*X15*2;

    //mess_state=(!X15)*(!X14)*(!X13)*0+(!X15)*(!X14)*(X13)*1+(!X15)*(X14)*1+(X15)*2;
    mess_state=C*(!X15)*(!X14)*(!X13)*0+C*(!X15)*(!X14)*(X13)*1+C*(!X15)*(X14)*1+(X15)*2+(!C)*3;


        /*if(deviceState.kt1_state==2)X12=true;
        else                        X12=false;
        if(deviceState.kt2_state==2)X11=true;
        else                        X11=false;
        if(deviceState.kt3_state==2)X10=true;
        else                        X10=false;
        if(deviceState.kt4_state==2)X9=true;
        else                        X9=false;
        if(deviceState.kt5_state==2)X8=true;
        else                        X8=false;
        if(deviceState.kt6_state==2)X7=true;
        else                        X7=false;
        if(deviceState.kt7_state==2)X6=true;
        else                        X6=false;
        if(deviceState.kt8_state==2)X5=true;
        else                        X5=false;
        if(deviceState.kt9_state==2)X4=true;
        else                        X4=false;
        if(deviceState.a_state==2)  X15=true;
        else                        X15=false;
        if(deviceState.b_state==2)  X14=true;
        else                        X14=false;
        if(deviceState.c_state==2)  X13=true;
        else                        X13=false;
        if(deviceState.k1_state==2) X16=true;
        else                        X16=false;
        if(deviceState.k2_state==2) X17=true;
        else                        X17=false;

        bool KH=(X15 &&  X16 || X15 &&  X12 || X14 &&  X11 || X13 &&  X10)&& X16;
        bool KB=(X15 &&  X17 || X15 &&  X9  || X14 &&  X8  || X13 && X7 || X15 &&  X6 || X14 &&  X5 || X13 &&  X4)&& X17;
        */

QSqlQuery *tqDeviceState = new QSqlQuery("select "
        "time_stamp, "  //0
        "a_state, "     //1
        "b_state, "     //2
        "c_state, "     //3
        "k1_state, "    //4
        "k2_state, "    //5
        "kt1_state, "   //6
        "kt2_state, "   //7
        "kt3_state, "   //8
        "kt4_state, "   //9
        "kt5_state, "   //10
        "kt6_state, "   //11
        "kt7_state, "   //12
        "kt8_state, "   //13
        "kt9_state, "   //14
        "door_state "   //15
        " from dev_state where device_id = "
        +QString::number(rec_id,10)+
        " order by rec_id desc limit 1");
//select time_stamp, a_state,b_state,c_state,k1_state,k2_state,kt1_state,kt2_state,kt3_state,kt4_state,kt5_state,kt6_state,kt7_state,kt8_state,kt9_state,door_state from dev_state where device_id = 728 order by rec_id desc limit 1;
    if (tqDeviceState->isActive())
    {
        //good
        tqDeviceState->next();
        deviceState.time_stamp = tqDeviceState->value(0).toDateTime();
        deviceState.a_state = tqDeviceState->value(1).toInt();
        deviceState.b_state = tqDeviceState->value(2).toInt();
        deviceState.c_state = tqDeviceState->value(3).toInt();
        deviceState.k1_state = tqDeviceState->value(4).toInt();
        deviceState.k2_state = tqDeviceState->value(5).toInt();
        deviceState.kt1_state = tqDeviceState->value(6).toInt();
        deviceState.kt2_state = tqDeviceState->value(7).toInt();
        deviceState.kt3_state = tqDeviceState->value(8).toInt();
        deviceState.kt4_state = tqDeviceState->value(9).toInt();
        deviceState.kt5_state = tqDeviceState->value(10).toInt();
        deviceState.kt6_state = tqDeviceState->value(11).toInt();
        deviceState.kt7_state = tqDeviceState->value(12).toInt();
        deviceState.kt8_state = tqDeviceState->value(13).toInt();
        deviceState.kt9_state = tqDeviceState->value(14).toInt();
        deviceState.door_state = tqDeviceState->value(15).toInt();

        //определение БПФ

        

        //mess_state
        // 1 - «БПФ Включен или аккумулятор»
        /*if((!X15) && (X14 || X13))
        {
            bpf_state=true;
            mess_state=0;                               // 0 - «БПФ Включен»
        }
        else
        {
            bpf_state=false;
            if(X15 || X14 || X13)   mess_state=3;       // 3 - «БПФ Выключен»
            else                    mess_state=2;       // 2 - «Аккумулятор включен»
        }*/
        /*if((!X15) && (X14 || X13) && (KH || KB))
        {
            bpf_state=true;
            mess_state=0;
        }
        else
        {
            if((!X15) && (X14 || X13))
            {
                bpf_state=true;
                mess_state=1;
            }
            else
            {
                bpf_state=false;
                if(X15 || X14 || X13)
                    mess_state=3;
                else
                    mess_state=2;
            }
        }*/
        //bpf_state=(!X15) && (X14 || X13);

        //"A"
        /*if((!bpf_state) && X15)
        {
            key=true;
            if((X15 || X14 || X13) && X16)  {bpf_ph_kn=5;}
            else                            {bpf_ph_kn=6;}
            if((X15 || X14 || X13) && X17)  {bpf_ph_kv=5;}
            else                            {bpf_ph_kv=6;}
        }
        //"B"
        if((bpf_state && X14) && (X16 || X17))
        {
            key=true;
            if((X15 || X14 || X13) && X16)  {bpf_ph_kn=1;}
            else                            {bpf_ph_kn=6;}
            if((X15 || X14 || X13) && X17)  {bpf_ph_kv=1;}
            else                            {bpf_ph_kv=6;}
        }
        
        //"C"
        if((bpf_state && (!X14) && X13) && (X16 || X17))
        {
            key=true;
            if((X15 || X14 || X13) && X16)  {bpf_ph_kn=3;}
            else                            {bpf_ph_kn=6;}
            if((X15 || X14 || X13) && X17)  {bpf_ph_kv=3;}
            else                            {bpf_ph_kv=6;}
        }
        if(!key)
        {
            bpf_ph_kn=6;
            bpf_ph_kv=6;
        }*/

    //здесь мы пока обработаем и нарисуем хуйню для заданного состояния КН и КВ
    //if(deviceState.k1_state==6) bpf_ph_kn=7;
    //if(deviceState.k2_state==6) bpf_ph_kv=7;
    }
    //////////////////////////////////////////////////////////////////////////

    delete recv_state_rawData;
    delete device_Configured;
    delete tqDeviceState;
}

void IPObject::saveData(){
    //store to BD if need
}

void IPObject::updateMainIcon(){
    setCol();
    QPainter iconPainter(monitorIcon);
    iconPainter.setBrush(QBrush(Qt::green));
    //Ручной или авто
    switch (deviceDescr.is_avto)
    {
    case 0: iconPainter.setBrush(QBrush(QColor(QRgb(rgb[0]))));//не определено
            break;
    case 1: iconPainter.setBrush(QBrush(QColor(QRgb(rgb[0]))));//ручник
            break;
    case 2: iconPainter.setBrush(QBrush(QColor(QRgb(rgb[1]))));//автоном
            break;
    default:iconPainter.setBrush(QBrush(QColor(QRgb(rgb[0]))));//не определено
            break;
    }

    iconPainter.drawRect(0,0,60,60);
    if (deviceDescr.is_root){
        //root shuno
        iconPainter.setBrush(QBrush(QColor(255,0,255)));
        iconPainter.drawRect(0,45,60,15);
    }
    //авария
    if (deviceDescr.is_error){
        iconPainter.setBrush(QBrush(QColor(QRgb(rgb[2]))));
        iconPainter.setPen(QColor(QRgb(rgb[2])));
        iconPainter.drawRect(0,0,5,60);
        iconPainter.drawRect(0,0,60,5);
        iconPainter.drawRect(0,55,60,60);
        iconPainter.drawRect(55,0,60,60);
    }else{
        if (deviceDescr.is_crc){
            iconPainter.setBrush(QBrush(QColor(QRgb(rgb[3]))));
            iconPainter.setPen(QColor(QRgb(rgb[3])));
            iconPainter.drawRect(0,0,5,60);
            iconPainter.drawRect(0,0,60,5);
            iconPainter.drawRect(0,55,60,60);
            iconPainter.drawRect(55,0,60,60);
        }
    }
    //Проверяем состояние К1
    iconPainter.setPen(QColor(0,0,0));
    switch(deviceState.k1_state){
        case 0://UNCN
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_uncn.png"));
            iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
        case 1://NONE
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_none.png"));
            iconPainter.setBrush(QBrush(QColor(255,255,255,0)));
            break;
        case 2://work_OK
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_work_ok.png"));
            iconPainter.setBrush(QBrush(QColor(QRgb(rgb[6]))));
            break;
        case 3://work_err
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_fail.png"));
            iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        case 4://off_OK
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_off_ok.png"));
            iconPainter.setBrush(QBrush(QColor(QRgb(rgb[5]))));
            break;
        case 5://off_err
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_fail.png"));
            iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        default :
            //detailPainter.drawImage(38-10,150-60,QImage(":/monitor/Resources/ip_k_uncn.png"));
            iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
    }
    iconPainter.drawEllipse(10,10,12,12);
    //Проверяем состояние К2
    switch(deviceState.k2_state){
        case 0://UNCN
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_uncn.png"));
            iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
        case 1://NONE
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_none.png"));
            iconPainter.setBrush(QBrush(QColor(255,255,255,0)));
            break;
        case 2://work_OK
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_work_ok.png"));
            iconPainter.setBrush(QBrush(QColor(QRgb(rgb[6]))));
            break;
        case 3://work_err
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_fail.png"));
            iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        case 4://off_OK
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_off_ok.png"));
            iconPainter.setBrush(QBrush(QColor(QRgb(rgb[5]))));
            break;
        case 5://off_err
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_fail.png"));
            iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        default :
            //detailPainter.drawImage(223-10,150-60,QImage(":/monitor/Resources/ip_k_uncn.png"));
            iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
    }
    iconPainter.drawEllipse(38,10,12,12);
    if (deviceDescr.door_present)
    {
        QRectF target(18.0, 13.0, 28.0, 28.0);
        QRectF source(0.0, 0.0, 10.0, 15.0);

        switch (deviceState.door_state)
        {
        case d_state_UNCR:
                {
                
                //iconPainter.drawImage(38,33,QImage(":/monitor/Resources/dver.png").scaled(20,20));
                iconPainter.setBrush(QBrush(QColor(192,192,192)));
                iconPainter.drawRect(38,33,12,15);
                iconPainter.setBrush(QBrush(QColor(0,0,0)));
                iconPainter.drawRect(42,40,1,2);
                break;
            }
        case d_state_NONE:
                {
                
                //iconPainter.drawImage(38,33,QImage(":/monitor/Resources/dver.png").scaled(20,20));
                iconPainter.setBrush(QBrush(QColor(192,192,192)));
                iconPainter.drawRect(38,33,12,15);
                iconPainter.setBrush(QBrush(QColor(0,0,0)));
                iconPainter.drawRect(42,40,1,2);
                break;
            }
        case d_state_CLOSED:
            {
                
                //iconPainter.drawImage(38,33,QImage(":/monitor/Resources/close.png").scaled(20,20));
                iconPainter.setBrush(QBrush(QColor(192,192,192)));
                iconPainter.drawRect(38,33,12,15);
                iconPainter.setBrush(QBrush(QColor(0,0,0)));
                iconPainter.drawRect(42,40,1,2);
                break;
            }
        case d_state_OPENED:
            {
                //iconPainter.setBrush(QBrush(QColor(250,0,0)));
            //  iconPainter.drawImage(38,33,QImage(":/monitor/Resources/open.png").scaled(20,20));
                iconPainter.setBrush(QBrush(QColor(250,0,0)));
                iconPainter.drawRect(38,33,12,15);
                iconPainter.setBrush(QBrush(QColor(0,0,0)));
                iconPainter.drawRect(42,40,1,2);
                break;
            }
        default:
                {
                
                //iconPainter.drawImage(38,33,QImage(":/monitor/Resources/dver.png").scaled(20,20));
                iconPainter.setBrush(QBrush(QColor(192,192,192)));
                iconPainter.drawRect(38,33,12,15);
                iconPainter.setBrush(QBrush(QColor(0,0,0)));
                iconPainter.drawRect(42,40,1,2);
                break;
            }
        }
        
    }
    if (!deviceDescr.is_viewed){
        iconPainter.drawImage(7,31,QImage(":/monitor/Resources/ahtung.png").scaled(20,20));
    }
    iconPainter.end();
    devButton->setName(deviceDescr.name);
    devButton->setFace(monitorIcon);
    devButton->setDescr(deviceDescr.notes);
}

void IPObject::updateImages(){
    setCol();
    updateMainIcon();
    QPainter detailPainter(details);
    
    if (deviceDescr.is_error){
        detailPainter.setBrush(QBrush(QColor(QRgb(rgb[4]))));
    }else{
        detailPainter.setBrush(QBrush(QColor(226,226,226)));
    }
    detailPainter.drawRect(0,0,529,329);
    
    /////////////////////////////////////////////////////////////////////////////////////////
    //Проверяем состояние фазы А
    switch(deviceState.a_state){
        case 0://UNCN
            detailPainter.drawImage(20-10,70-60,QImage(":/monitor/Resources/ip_ph_uncn.png"));
            break;
        case 1://NC
            //detailPainter.drawImage(20-10,70-60,QImage(":/monitor/Resources/ip_ph_none.png"));
            break;
        case 2://OK
            detailPainter.drawImage(20-10,70-60,QImage(":/monitor/Resources/ip_ph_ok.png"));
            break;
        case 3://ERROR авария, нет фазы
            detailPainter.drawImage(20-10,70-60,QImage(":/monitor/Resources/ip_ph_err.png"));
            break;
        case 4://ERROR ошибка в маске
            detailPainter.drawImage(20-10,70-60,QImage(":/monitor/Resources/ip_ph_on_err.png"));
            break;
        default://
            detailPainter.drawImage(20-10,70-60,QImage(":/monitor/Resources/ip_ph_uncn.png"));
            break;
    }
    if (deviceState.a_state != 1){
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(10,60,tr("А"));
    }
    //Проверяем состояние фазы В
    switch(deviceState.b_state){
        case 0://UNCN
            detailPainter.drawImage(80-10,70-60,QImage(":/monitor/Resources/ip_ph_uncn.png"));
            break;
        case 1://NC
            //detailPainter.drawImage(80-10,70-60,QImage(":/monitor/Resources/ip_ph_none.png"));
            break;
        case 2://OK
            detailPainter.drawImage(80-10,70-60,QImage(":/monitor/Resources/ip_ph_ok.png"));
            break;
        case 3://ERROR
            detailPainter.drawImage(80-10,70-60,QImage(":/monitor/Resources/ip_ph_err.png"));
            break;
        case 4://ERROR
            detailPainter.drawImage(80-10,70-60,QImage(":/monitor/Resources/ip_ph_on_err.png"));
            break;
        default://
            detailPainter.drawImage(80-10,70-60,QImage(":/monitor/Resources/ip_ph_uncn.png"));
            break;
    }
    if (deviceState.b_state != 1){
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(70,60,tr("В"));
    }
    //Проверяем состояние фазы С
    switch(deviceState.c_state){
        case 0://UNCN
            detailPainter.drawImage(140-10,70-60,QImage(":/monitor/Resources/ip_ph_uncn.png"));
            break;
        case 1://NC
            //detailPainter.drawImage(140-10,70-60,QImage(":/monitor/Resources/ip_ph_none.png"));
            break;
        case 2://OK
            detailPainter.drawImage(140-10,70-60,QImage(":/monitor/Resources/ip_ph_ok.png"));
            break;
        case 3://ERROR
            detailPainter.drawImage(140-10,70-60,QImage(":/monitor/Resources/ip_ph_err.png"));
            break;
        case 4://ERROR
            detailPainter.drawImage(140-10,70-60,QImage(":/monitor/Resources/ip_ph_on_err.png"));
            break;
        default://
            detailPainter.drawImage(140-10,70-60,QImage(":/monitor/Resources/ip_ph_uncn.png"));
            break;
    }
    if (deviceState.c_state != 1){
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(130,60,tr("С"));
    }
    //Проверяем состояние К1
    //iconPainter.setPen(QColor(0,0,0));
    switch(deviceState.k1_state){
        case 0://UNCN
            detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_uncn_new.png"));
            //iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
        case 1://NONE
            //detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_none.png"));
            //iconPainter.setBrush(QBrush(QColor(255,255,255,0)));
            break;
        case 2://work_OK
            detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_work_ok_new.png"));//##########
            //iconPainter.setBrush(QBrush(QColor(209,224,94)));
            break;
        case 3://work_err
            detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_fail_on_new.png"));
            //iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        case 4://off_OK
            detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_off_ok_new.png"));
            //iconPainter.setBrush(QBrush(QColor(59,48,56)));
            break;
        case 5://off_err
            detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_fail_off_new.png"));
            //iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        default :
            detailPainter.drawImage(78-10,150-60,QImage(":/monitor/Resources/ip_k_uncn_new.png"));
            //iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
    }
    if (deviceState.k1_state != 1)
    {
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(68+25,135,tr("КН"));
    }
    //БПФ
    //KH
    //if (deviceState.k1_state != 1)
    //{
    switch(bpf_ph_kn)
    {
    case 1:
        //detailPainter.drawImage(18,112,QImage("./Resources/bpf_none.png"));
        break;
    case 2:
        detailPainter.drawImage(18,112,QImage("./Resources/bpf_a_act.png"));
        break;
    case 3:
        detailPainter.drawImage(18,112,QImage("./Resources/bpf_b_act.png"));
        break;
    case 4:
        detailPainter.drawImage(18,112,QImage("./Resources/bpf_c_act.png"));
        break;
    case 5:
        detailPainter.drawImage(18,112,QImage("./Resources/bpf_none.png"));
        break;
    case 6:
        detailPainter.drawImage(18,112,QImage("./Resources/bpf_must_on.png"));
        break;
    default:
        break;
    }
    //}



    //Проверяем состояние К2
    switch(deviceState.k2_state){
        case 0://UNCN
            detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_uncn_new.png"));
            //iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
        case 1://NONE
            //detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_none.png"));
            //iconPainter.setBrush(QBrush(QColor(255,255,255,0)));
            break;
        case 2://work_OK
            detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_work_ok_new.png"));
            //iconPainter.setBrush(QBrush(QColor(209,224,94)));
            break;
        case 3://work_err
            detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_fail_on_new.png"));
            //iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        case 4://off_OK
            detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_off_ok_new.png"));
            //iconPainter.setBrush(QBrush(QColor(59,48,56)));
            break;
        case 5://off_err
            detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_fail_off_new.png"));
            //iconPainter.setBrush(QBrush(QColor(240,22,32)));
            break;
        default :
            detailPainter.drawImage(223+70,150-60,QImage(":/monitor/Resources/ip_k_uncn_new.png"));
            //iconPainter.setBrush(QBrush(QColor(192,192,192)));
            break;
    }
    if (deviceState.k2_state != 1){
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(320,135,tr("КВ"));
    }
//  if (deviceState.k2_state != 1)
//  {
    //KB
    switch(bpf_ph_kv)
    {
    case 1:
        //detailPainter.drawImage(243,112,QImage("./Resources/bpf_none.png"));
        break;
    case 2:
        detailPainter.drawImage(243,112,QImage("./Resources/bpf_a_act.png"));
        break;
    case 3:
        detailPainter.drawImage(243,112,QImage("./Resources/bpf_b_act.png"));
        break;
    case 4:
        detailPainter.drawImage(243,112,QImage("./Resources/bpf_c_act.png"));
        break;
    case 5:
        detailPainter.drawImage(243,112,QImage("./Resources/bpf_none.png"));
        break;
    case 6:
        detailPainter.drawImage(243,112,QImage("./Resources/bpf_must_on.png"));
        break;
    default:
        break;
    }
//  }

                //mess_state БПФ
    switch (mess_state)
    {
        // 0 - «БПФ Включен»    
        case 0:
                detailPainter.drawText(288,40,tr("Аккумулятор включён"));
                break;
                // 1 - «БПФ Включен или аккумулятор»
        case 1:
                detailPainter.drawText(247,40,tr("БПФ включён"));
                break;
                // 2 - «Аккумулятор включен»
        //case 2:
                //detailPainter.drawText(250,40,tr("БПФ отключён"));
                //break;
        //case 3:
                // 3 - «БПФ Выключен»
                //detailPainter.drawText(253,40,tr("БПФ отключен"));
                //break;
        default:
                break;
    }


    //iconPainter.drawEllipse(38,10,12,12);

    


    bool paintConnections;
    //что есть КТ1?????
    paintConnections = TRUE;
    if (deviceConfig[0].type == 0){
        //FUSE
        switch(deviceState.kt1_state){
            case 0://UNCN
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt1_state){
            case 0://UNCN
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                break;
            case 2://work ok
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(20-10,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //1
        detailPainter.setPen(QPen(QBrush(QColor(0xFF,0xDE,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(37-10,118-60,37-10,125-60);//V
        detailPainter.drawLine(37-10,125-60,95-10+69,125-60);//G
        detailPainter.drawLine(95-10+69,125-60,95+69-10,150-60);//V
        detailPainter.drawLine(95-10+69,228-60,95-10+69,245-60);//V
        detailPainter.drawLine(95-10+69,245-60,37-10,245-60);//G
        detailPainter.drawLine(37-10,245-60,37-10,270-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25,320,tr("1"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ2?????
    paintConnections = TRUE;
    if (deviceConfig[1].type == 0){
        //FUSE
        switch(deviceState.kt2_state){
            case 0://UNCN
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt2_state){
            case 0://UNCN
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                break;
            case 2://work ok
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(60+5,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //2
        detailPainter.setPen(QPen(QBrush(QColor(0x1C,0xA0,0x0E)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(117-10+69,228-60,117-10+69,270-60-15);//V
        detailPainter.drawLine(117-10+69,270-60-15,117-10+69-94,270-60-15);//G
        detailPainter.drawLine(77-10+69-54,270-60-15,77-10+69-54,270-60);//V
        detailPainter.drawLine(97-10,118-60,97-10,135-60);//V
        detailPainter.drawLine(117-10+69,135-60,117-10+69-89,135-60);//G
        detailPainter.drawLine(117-10+69,135-60,117-10+69,150-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+55,320,tr("2"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ3?????
    paintConnections = TRUE;
    if (deviceConfig[2].type == 0){
        //FUSE
        switch(deviceState.kt3_state){
            case 0://UNCN
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt3_state){
            case 0://UNCN
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                break;
            case 2://work ok
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(100+20,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //3
        detailPainter.setPen(QPen(QBrush(QColor(0xCC,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(157-10,118-60,157-10,145-60);//V
        detailPainter.drawLine(197,145-60,157-10,145-60);//G
        detailPainter.drawLine(139-10+69,145-60,139-10+69,150-60);//V
        detailPainter.drawLine(139-10+69,228-60,139-10+69,245-60+20);//V
        detailPainter.drawLine(139-10+69,245-60+20,99-10+69-21,245-60+20);//G
        detailPainter.drawLine(99-10+69-21,245-60+20,99-10+69-21,245-60+20+5);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+110,320,tr("3"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ4?????
    paintConnections = TRUE;
    if (deviceConfig[3].type == 0){
        //FUSE
        switch(deviceState.kt4_state){
            case 0://UNCN
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt4_state){
            case 0://UNCN
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(140+70,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //4
        detailPainter.setPen(QPen(QBrush(QColor(0xFF,0xDE,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(37-10,118-60,37-10,125-60);//V
        detailPainter.drawLine(37-10,125-60,240-10+149,125-60);//G
        detailPainter.drawLine(240-10+149,125-60,240-10+149,150-60);//V
        detailPainter.drawLine(240-10+149,228-60,240-10+149,238-60);//V
        detailPainter.drawLine(240-10+149,238-60,157-10+149-69,238-60);//G
        detailPainter.drawLine(157-10+149-69,238-60,157-10+149-69,270-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+200,320,tr("4"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ5?????
    paintConnections = TRUE;
    if (deviceConfig[4].type == 0){
        //FUSE
        switch(deviceState.kt5_state){
            case 0://UNCN
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt5_state){
            case 0://UNCN
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(180+85,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //5
        detailPainter.setPen(QPen(QBrush(QColor(0x1C,0xA0,0x0E)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(97-10,118-60,97-10,135-60);//V
        detailPainter.drawLine(97-10,135-60,262-10+149,135-60);//G
        detailPainter.drawLine(262-10+149,135-60,262-10+149,150-60);//V
        detailPainter.drawLine(262-10+149,228-60,262-10+149,250-60);//V
        detailPainter.drawLine(262-10+149,250-60,197-10+149-54,250-60);//G
        detailPainter.drawLine(197-10+149-54,250-60,197-10+149-54,270-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+255,320,tr("5"));
        //////////////////////////////////////////////////////////////////////////

    }
    //что есть КТ6?????
    paintConnections = TRUE;
    if (deviceConfig[5].type == 0){
        //FUSE
        switch(deviceState.kt6_state){
            case 0://UNCN
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt6_state){
            case 0://UNCN
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(220+100,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //6
        detailPainter.setPen(QPen(QBrush(QColor(0xCC,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(157-10,118-60,157-10,145-60);//V
        detailPainter.drawLine(157-10,145-60,284-10+149,145-60);//G
        detailPainter.drawLine(284-10+149,145-60,284-10+149,150-60);//V
        detailPainter.drawLine(284-10+149,228-60,284-10+149,262-60);//V
        detailPainter.drawLine(284-10+149,262-60,237-10+149-39,262-60);//G
        detailPainter.drawLine(237-10+149-39,262-60,237-10+149-39,270-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+310,320,tr("6"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ7?????
    paintConnections = TRUE;
    if (deviceConfig[6].type == 0){
        //FUSE
        switch(deviceState.kt7_state){
            case 0://UNCN
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt7_state){
            case 0://UNCN
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(260+115,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //7
        detailPainter.setPen(QPen(QBrush(QColor(0xFF,0xDE,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(37-10,118-60,37-10,125-60);//V
        detailPainter.drawLine(37-10,125-60,240-10+149,125-60);//G
        detailPainter.drawLine(240-10+149,125-60,240-10+149,150-60);//V
        detailPainter.drawLine(240-10+149,228-60,240-10+149,238-60);//V
        detailPainter.drawLine(240-10+149,238-60,277-10+149-24,238-60);//G
        detailPainter.drawLine(277-10+149-24,238-60,277-10+149-24,238-60+30);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+365,320,tr("7"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ8?????
    paintConnections = TRUE;
    if (deviceConfig[7].type == 0){
        //FUSE
        switch(deviceState.kt8_state){
            case 0://UNCN
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt8_state){
            case 0://UNCN
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(300+130,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //8
        detailPainter.setPen(QPen(QBrush(QColor(0x1C,0xA0,0x0E)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(97-10,118-60,97-10,135-60);//V
        detailPainter.drawLine(97-10,135-60,262-10+149,135-60);//G
        detailPainter.drawLine(262-10+149,135-60,262-10+149,150-60);//V
        detailPainter.drawLine(262-10+149,228-60,262-10+149,250-60);//V
        detailPainter.drawLine(262-10+149,250-60,317-10+149-9,250-60);//G
        detailPainter.drawLine(317-10+149-9,250-60,317-10+149-9,270-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+420,320,tr("8"));
        //////////////////////////////////////////////////////////////////////////
    }
    //что есть КТ9?????
    paintConnections = TRUE;
    if (deviceConfig[8].type == 0){
        //FUSE
        switch(deviceState.kt9_state){
            case 0://UNCN
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_on_ok.png"));//##########
                break;
            case 3://work - fail
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_fail.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_feedback.png"));
                break;
            case 6://work fail
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_on_fail.png"));//##########
                break;
            default:
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }else{
        //FEED_BACK
        paintConnections = FALSE;
        switch(deviceState.kt9_state){
            case 0://UNCN
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_feedback_uncn.png"));
                break;
            case 1://none
                //detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_none.png"));
                paintConnections = FALSE;
                break;
            case 2://work ok
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_feedback_on_ok.png"));
                break;
            case 3://work - fail
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            case 4://off ok
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_feedback_off_ok.png"));
                break;
            case 5://off - fail
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_feedback_err.png"));
                break;
            default:
                detailPainter.drawImage(340+145,270-60,QImage(":/monitor/Resources/ip_f_uncn.png"));
                break;
        }
    }
    if (paintConnections){
        //9
        detailPainter.setPen(QPen(QBrush(QColor(0xCC,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawLine(157-10,118-60,157-10,145-60);//V
        detailPainter.drawLine(157-10,145-60,284-10+149,145-60);//G
        detailPainter.drawLine(284-10+149,145-60,284-10+149,150-60);//V
        detailPainter.drawLine(284-10+149,228-60,284-10+149,262-60);//V
        detailPainter.drawLine(284-10+149,262-60,357-10+149+6,262-60);//G
        detailPainter.drawLine(357-10+149+6,262-60,357-10+149+6,270-60);//V
        detailPainter.setPen(QPen(QBrush(QColor(0x00,0x00,0x00)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        detailPainter.drawText(25+475,320,tr("9"));
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////

    

    //door
    detailPainter.setBrush(QBrush(Qt::black));
    detailPainter.setPen(Qt::black);
    /*if (deviceDescr.door_present){
        switch (deviceState.door_state){
            case d_state_UNCR:
                detailPainter.drawText(250,25,tr("Состояние двери неизвестно"));
                break;
            case d_state_NONE:
                detailPainter.drawText(250,25,tr("Дверь отсутствует"));
                break;
            case d_state_CLOSED:
                //detailPainter.drawImage(250,25,QImage(":/monitor/Resources/close.png"));
                detailPainter.drawText(250,25,tr("Дверь закрыта"));
                break;
            case d_state_OPENED:
                //detailPainter.drawImage(250,25,QImage(":/monitor/Resources/open.png"));
                detailPainter.drawText(250,25,tr("Дверь открыта"));
                break;
            default:
                detailPainter.drawText(250,25,tr("Состояние двери неизвестно"));
                break;
            //closed
        }
    }else{
        //absent
        detailPainter.drawText(250,25,tr("Дверь отсутствует"));
    }*/

    //auto/manual
    //error/OK/CRC
    //viewed/not viewed
    /*/DEBUG
        //iconPainter.setBrush(QBrush(Qt::red));
        detailPainter.setBrush(QBrush(Qt::red));
        //iconPainter.drawEllipse(31,31,10,10);
        detailPainter.drawEllipse(190,170,100,100);
    //END of DEBUG*/


    //---------------------------------------------------------//

    //индикация уровня сигнала GSM

    if(key_anim)
    {
    //lavel =10;

    int k_x,k_y,x,y;
    int w,h;
    int M1,M2,M3,M0;
    int start_x,start_y;
    QRectF rectangle;
    QRect rect_lb[6],rect;
    QString time;

    if (lavel>30)
        lavel=30;
    M0=0;M1=9;M2=13;M3=30;

    start_x=450;
    start_y=40;
    k_x=8;
    k_y=4;

    
    
    QDateTime currentTime = QDateTime::currentDateTime();
    time = currentTime.toString("HH:mm:ss dd.MM.yy").toUtf8().constData();
    // old time=currentTime.toString("HH:mm:ss dd.MM.yy").toAscii().constData();
    
    detailPainter.setBrush(QBrush(Qt::gray));
    detailPainter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    /*rectangle.setCoords(445.0, 5.0, 505.0, 40.0);
    detailPainter.drawRect(rectangle);*/

    //отображение индикатора
    rect_lb[0].setCoords(start_x,start_y-4*k_y,start_x+k_x,start_y);
    detailPainter.drawRect(rect_lb[0]);
    x=start_x;
    y=start_y-4*k_y;
    for(int i=1;i<6;i++)
    {
        rect_lb[i].setCoords(x+i*k_x,y-i*k_y,x+(i+1)*k_x,start_y);
        detailPainter.drawRect(rect_lb[i]);
    }
    
    //отображение уровня
    if(lavel>M0 && lavel<=M0+(M1-M0)/2)
    {
        rect.setCoords(rect_lb[0].left()+1,rect_lb[0].top()+1,rect_lb[0].right()-1,rect_lb[0].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xCC,0x00,0x00)));
    }
    else if(lavel>M0+(M1-M0)/2 && lavel<=M1)
    {
        rect.setCoords(rect_lb[0].left()+1,rect_lb[0].top()+1,rect_lb[0].right()-1,rect_lb[0].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xCC,0x00,0x00)));
        rect.setCoords(rect_lb[1].left()+1,rect_lb[1].top()+1,rect_lb[1].right()-1,rect_lb[1].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xCC,0x00,0x00)));
    }
    else if(lavel>M1 && lavel<=M1+(M2-M1)/2)
    {
        rect.setCoords(rect_lb[0].left()+1,rect_lb[0].top()+1,rect_lb[0].right()-1,rect_lb[0].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
        rect.setCoords(rect_lb[1].left()+1,rect_lb[1].top()+1,rect_lb[1].right()-1,rect_lb[1].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
        rect.setCoords(rect_lb[2].left()+1,rect_lb[2].top()+1,rect_lb[2].right()-1,rect_lb[2].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
    }
    else if(lavel>M1+(M2-M1)/2 && lavel<=M2)
    {
        rect.setCoords(rect_lb[0].left()+1,rect_lb[0].top()+1,rect_lb[0].right()-1,rect_lb[0].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
        rect.setCoords(rect_lb[1].left()+1,rect_lb[1].top()+1,rect_lb[1].right()-1,rect_lb[1].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
        rect.setCoords(rect_lb[2].left()+1,rect_lb[2].top()+1,rect_lb[2].right()-1,rect_lb[2].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
        rect.setCoords(rect_lb[3].left()+1,rect_lb[3].top()+1,rect_lb[3].right()-1,rect_lb[3].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0xFF,0xDE,0x00)));
    }
    else if(lavel>M2 && lavel<=M2+(M3-M2)/2)
    {
        rect.setCoords(rect_lb[0].left()+1,rect_lb[0].top()+1,rect_lb[0].right()-1,rect_lb[0].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[1].left()+1,rect_lb[1].top()+1,rect_lb[1].right()-1,rect_lb[1].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[2].left()+1,rect_lb[2].top()+1,rect_lb[2].right()-1,rect_lb[2].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[3].left()+1,rect_lb[3].top()+1,rect_lb[3].right()-1,rect_lb[3].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[4].left()+1,rect_lb[4].top()+1,rect_lb[4].right()-1,rect_lb[4].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
    }
    else if(lavel>M2+(M3-M2)/2)
    {
        rect.setCoords(rect_lb[0].left()+1,rect_lb[0].top()+1,rect_lb[0].right()-1,rect_lb[0].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[1].left()+1,rect_lb[1].top()+1,rect_lb[1].right()-1,rect_lb[1].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[2].left()+1,rect_lb[2].top()+1,rect_lb[2].right()-1,rect_lb[2].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[3].left()+1,rect_lb[3].top()+1,rect_lb[3].right()-1,rect_lb[3].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[4].left()+1,rect_lb[4].top()+1,rect_lb[4].right()-1,rect_lb[4].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
        rect.setCoords(rect_lb[5].left()+1,rect_lb[5].top()+1,rect_lb[5].right(),rect_lb[5].bottom());
        detailPainter.fillRect (rect,QBrush(QColor(0x1C,0xA0,0x0E)));
    }


    //отображение числового значения
    detailPainter.drawText(430,53,time);
    time.clear();
    detailPainter.drawText(437,65,tr("Качество"));
    detailPainter.drawText(437,77,tr("   связи"));
    time.clear();
    time=QString::number((lavel*100)/30,10)+tr(" %");
    detailPainter.drawText(490,72,time);
    }
 //----------------------------------------------------------------------//
    detailPainter.end();
    /*BaseSettings *bs = BaseSettings::createInstance();
    if (bs->ipW->isVisible()){
        if (bs->ipW->getObject() == this){
            bs->ipW->iShow(this);
        }
    }
    bs->freeInst();*/
}

int IPObject::getGroupG(){
    return deviceDescr.group_g;
}

int IPObject::getGroupA(){
    return deviceDescr.group_a;
}

int IPObject::getGroupS(){
    return deviceDescr.is_error;
}

DeviceButton* IPObject::getButton(){
    return devButton;
}

void IPObject::setCol()
{
    BaseSettings *bs = BaseSettings::createInstance();
    int colorsCount = bs->settings->value("colors/col_count","0").toInt();
    for (int i=0;i<colorsCount;i++)
    {
        rgb[i]=bs->settings->value("colors/color_"+QString::number(i,10)+"_value","0").toUInt();
    }
    bs->freeInst();
}

/*void IPObject::getDeviceClick(IPObject * master)
{
    emit deviceToIPW(master);
}*/
