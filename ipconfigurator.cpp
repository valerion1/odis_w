#include "ipconfigurator.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QTimer>
#include <QKeyEvent>
#include "monitor.h"
#include "ipwindow.h"
#include "ipobject.h"


IPConfigurator::IPConfigurator(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//QSettings counter_settings(QApplication::applicationDirPath() + "/counters.ini",QSettings::IniFormat);

	//cb_door=NULL;
	//cb_door = new QCheckBox(ui.groupBox_2);
	//cb_door->setText(tr("Дверь"));
    //ui.vboxLayout->addWidget(cb_door);
	

	on_pbDefault_clicked();
	//load_from_base
	comboArray[0] = ui.combo_1;
	comboArray[1] = ui.combo_2;
	comboArray[2] = ui.combo_3;
	comboArray[3] = ui.combo_4;
	comboArray[4] = ui.combo_5;
	comboArray[5] = ui.combo_6;
	comboArray[6] = ui.combo_7;
	comboArray[7] = ui.combo_8;
	comboArray[8] = ui.combo_9;
	checkArray[0] = ui.cb_1;
	checkArray[1] = ui.cb_2;
	checkArray[2] = ui.cb_3;
	checkArray[3] = ui.cb_4;
	checkArray[4] = ui.cb_5;
	checkArray[5] = ui.cb_6;
	checkArray[6] = ui.cb_7;
	checkArray[7] = ui.cb_8;
	checkArray[8] = ui.cb_9;
	lineEditArray[0] = ui.le_1;
	lineEditArray[1] = ui.le_2;
	lineEditArray[2] = ui.le_3;
	lineEditArray[3] = ui.le_4;
	lineEditArray[4] = ui.le_5;
	lineEditArray[5] = ui.le_6;
	lineEditArray[6] = ui.le_7;
	lineEditArray[7] = ui.le_8;
	lineEditArray[8] = ui.le_9;


	//tm_state= new QTimer();
	//connect(tm_state,SIGNAL(timeout()),this,SLOT(tm_stateTimer()));
	//state_inter=5;
	key_config=true;
	key_button=true;
	
}

IPConfigurator::~IPConfigurator()
{
	//if(cb_datamode) delete cb_datamode;
	//if(cb_door) delete cb_door;
	//if(tm_state) delete tm_state;
}

void IPConfigurator::setParent(IPWindow *ptr)
{
	parent=(IPWindow*)ptr;
}
//////////////////////////////////////////////////////////////////////////

void IPConfigurator::keyPressEvent(QKeyEvent *event){
	if (event->key() == Qt::Key_Escape){
		//tm_state->stop();
		hide();
	}
}

void IPConfigurator::IShow(int bdIndex)
{
	//mainBs = BaseSettings::createInstance();
	curIndex = bdIndex;
	on_pbDefault_clicked();
	//------------------------------------------------
	//заполнить комбобокс типами счётчика из ini файла
	//------------------------------------------------
	if(key_button)
		ui.pbOK->setText(tr("Передать конфигурацию"));
	else
		ui.pbOK->setText(tr("Применить"));
	//
	//clear combo boxes
	ui.combo_1->clear();
	ui.combo_2->clear();
	ui.combo_3->clear();
	ui.combo_4->clear();
	ui.combo_5->clear();
	ui.combo_6->clear();
	ui.combo_7->clear();
	ui.combo_8->clear();
	ui.combo_9->clear();
	//fill combo boxes
	QSqlQuery *tq = new QSqlQuery("select rec_id, raw_val, human_descr from check_point_type");
	if (tq->isActive())
	{
		while (tq->next())
		{
			ui.combo_1->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_2->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_3->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_4->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_5->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_6->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_7->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_8->addItem(tq->value(2).toString(),tq->value(1).toInt());
			ui.combo_9->addItem(tq->value(2).toString(),tq->value(1).toInt());
		}
		show();
	}
	//B C K1 K2
	tq->exec("select b_present, c_present, k1_present, k2_present, door_present, data_mode from device where rec_id = "
		+QString::number(bdIndex,10));
	if (tq->isActive()){
		while (tq->next()){
			b = tq->value(0).toBool();
			c = tq->value(1).toBool();
			k1 = tq->value(2).toBool();
			k2 = tq->value(3).toBool();
			door = tq->value(4).toBool();
			data_mode = tq->value(5).toBool();
			//counter_pr esent=tq->value(6).toBool();//не используется, вместо него counter_toSend

			//counter_number=tq->value(6).toInt();
			//counter_type=tq->value(7).toInt();
			//counter_pass=parent->master->deviceDescr.counter_pass;
		}
	}
	ui.cb_B->setCheckState((b)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_C->setCheckState((c)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_K1->setCheckState((k1)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_K2->setCheckState((k2)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_door->setCheckState((door)?(Qt::Checked):(Qt::Unchecked));
	ui.lineEdit_countSerial->setText(QString::number(parent->master->deviceDescr.counter_number));
	ui.comboBox_countType->setEditable(false);
	ui.comboBox_countType->setEnabled(false);
	ui.comboBox_countType->setStyleSheet("QComboBox::drop-down {size:0px;}");
	ui.lineEdit_countSerial->setEnabled(false);
	ui.lineEdit_countPass->setEnabled(false);

	int temp_counter_size=parent->parent->mainBs->settings->beginReadArray("counters");
	for (int i=0;i<temp_counter_size;i++)
	{
		parent->parent->mainBs->settings->setArrayIndex(i);
		ui.comboBox_countType->addItem(parent->parent->mainBs->settings->value("name").toString());//заполняем комбобокс с наименованиями счётчиков
	}
	parent->parent->mainBs->settings->endArray();
	ui.comboBox_countType->setCurrentIndex(parent->master->deviceDescr.counter_type);
	ui.lineEdit_countPass->setText(QString::number(parent->master->deviceDescr.counter_pass));

	tq->exec("select rec_id, ckeck_point_number, k_number, phase_number, human_name, "
		"type, is_present from shuno_config where device_id = "+QString::number(bdIndex,10));
	int i=0;
	if (tq->isActive()){
		while (tq->next()){
			tCp[i] = tq->value(6).toBool();
			checkArray[i]->setCheckState((tq->value(6).toBool())?(Qt::Checked):(Qt::Unchecked));
			comboArray[i]->setCurrentIndex(comboArray[i]->findData(tq->value(5).toInt()));
			lineEditArray[i]->setText(tq->value(4).toString());
			//indexes[i] = tq->value(0).toInt();
			i++;
		}
	}
	//remember old values
	old_b   = b; 
	old_c   = c;
	old_k1  = k1;
	old_k2  = k2;
	old_cp1 = tCp[0];
	old_cp2 = tCp[1];
	old_cp3 = tCp[2];
	old_cp4 = tCp[3];
	old_cp5 = tCp[4];
	old_cp6 = tCp[5];
	old_cp7 = tCp[6];
	old_cp8 = tCp[7];
	old_cp9 = tCp[8];
	old_door= door;
	old_datamode= data_mode;
	delete tq;
	
	//ui.horizontalLayout_counters->hi
	//if (!key_login)
	//if ()
	//{
	ui.label_countType->setHidden(!parent->master->deviceDescr.counter_toSend);
	ui.label_countSerial->setHidden(!parent->master->deviceDescr.counter_toSend);
	ui.label_countPass->setHidden(!parent->master->deviceDescr.counter_toSend);
	ui.comboBox_countType->setHidden(!parent->master->deviceDescr.counter_toSend);
	ui.lineEdit_countSerial->setHidden(!parent->master->deviceDescr.counter_toSend);
	ui.lineEdit_countPass->setHidden(!parent->master->deviceDescr.counter_toSend);
	//}
}

//////////////////////////////////////////////////////////////////////////
void IPConfigurator::on_pbDefault_clicked()
{
	ui.cb_B->setCheckState(Qt::Checked);
	ui.cb_C->setCheckState(Qt::Checked);
	ui.cb_K1->setCheckState(Qt::Checked);
	ui.cb_K2->setCheckState(Qt::Checked);
	ui.cb_1->setCheckState(Qt::Checked);
	ui.cb_2->setCheckState(Qt::Checked);
	ui.cb_3->setCheckState(Qt::Checked);
	ui.cb_4->setCheckState(Qt::Checked);
	ui.cb_5->setCheckState(Qt::Checked);
	ui.cb_6->setCheckState(Qt::Checked);
	ui.cb_7->setCheckState(Qt::Checked);
	ui.cb_8->setCheckState(Qt::Checked);
	ui.cb_9->setCheckState(Qt::Checked);
	//cb_datamode->setCheckState(Qt::Checked);
	ui.cb_door->setCheckState(Qt::Checked);
}

/*
void IPConfigurator::tm_stateTimer()
{
	//save
	QSqlQuery *tq = new QSqlQuery();
	QString str;

	//Перерисовка конфигурации
	str.clear();
	str="select rec_id,command_type,command_state from send where device_id="
		+QString::number(curIndex,10)+" and command_type=17 order by rec_id desc limit 1";
	if(tq->exec(str))
	{
		if(tq->isActive())
		{
			tq->next();
			if(tq->value(1).toInt()==17 && tq->value(2).toInt()==0)
			{
				tm_state->stop();
				//////////////////////////////////////////////////////////////////////////
				QSqlQuery *tqDeviceState = new QSqlQuery("select * from dev_state where device_id = "
					+QString::number(curIndex,10)+
					" order by rec_id desc limit 1");
				if (!tqDeviceState->isActive())
				{
					QMessageBox::critical(0, qApp->tr("Ошибка отправки конфигурации"),
						tr("Ошибка"), QMessageBox::Cancel,
						QMessageBox::NoButton);
					delete tqDeviceState;
					delete tq;
					return;
				}
				tqDeviceState->next();
				tq->prepare("update dev_state set "
					"b_state   = :b_state, "
					"c_state   = :c_state, "
					"k1_state  = :k1_state, "
					"k2_state  = :k2_state, "
					"kt1_state = :kt1_state, "
					"kt2_state = :kt2_state, "
					"kt3_state = :kt3_state, "
					"kt4_state = :kt4_state, "
					"kt5_state = :kt5_state, "
					"kt6_state = :kt6_state, "
					"kt7_state = :kt7_state, "
					"kt8_state = :kt8_state, "
					"kt9_state = :kt9_state "
					"where rec_id = "+QString::number(tqDeviceState->value(0).toInt(),10));
				//////////////////////////////////////////////////////////////////////////
				if (old_b != b)
				{
					if (b)
					{
						tq->bindValue(":b_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":b_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":b_state",tqDeviceState->value(4));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_c != c)
				{
					if (c)
					{
						tq->bindValue(":c_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":c_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":c_state",tqDeviceState->value(5));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_k1 != k1)
				{
					if (k1)
					{
						tq->bindValue(":k1_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":k1_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":k1_state",tqDeviceState->value(6));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_k2 != k2)
				{
					if (k2)
					{
						tq->bindValue(":k2_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":k2_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":k2_state",tqDeviceState->value(7));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp1 != cp1)
				{
					if (cp1)
					{
						tq->bindValue(":kt1_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt1_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt1_state",tqDeviceState->value(8));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp2 != cp2)
				{
					if (cp2)
					{
						tq->bindValue(":kt2_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt2_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt2_state",tqDeviceState->value(9));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp3 != cp3)
				{
					if (cp3)
					{
						tq->bindValue(":kt3_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt3_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt3_state",tqDeviceState->value(10));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp4 != cp4)
				{
					if (cp4)
					{
						tq->bindValue(":kt4_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt4_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt4_state",tqDeviceState->value(11));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp5 != cp5)
				{
					if (cp5)
					{
						tq->bindValue(":kt5_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt5_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt5_state",tqDeviceState->value(12));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp6 != cp6)
				{
					if (cp6)
					{
						tq->bindValue(":kt6_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt6_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt6_state",tqDeviceState->value(13));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp7 != cp7)
				{
					if (cp7)
					{
						tq->bindValue(":kt7_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt7_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt7_state",tqDeviceState->value(14));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp8 != cp8)
				{
					if (cp8)
					{
						tq->bindValue(":kt8_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt8_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt8_state",tqDeviceState->value(15));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				if (old_cp9 != cp9)
				{
					if (cp9)
					{
						tq->bindValue(":kt9_state",0);//UNCR
					}
					else
					{
						tq->bindValue(":kt9_state",1);//NONE
					}
				}
				else
				{
					tq->bindValue(":kt9_state",tqDeviceState->value(16));//Previous val
				}
				//////////////////////////////////////////////////////////////////////////
				tq->exec();
				delete tqDeviceState;
				emit refreshG();
				
			}
			else if(tq->value(1).toInt()==17 && tq->value(2).toInt()==11)
			{
				tm_state->stop();
			}
		}
	}
	
	delete tq;
}*/

void IPConfigurator::on_pbOK_clicked()
{
	quint32 mask=0x80000;
	//QSqlQuery *tq = new QSqlQuery();
	//QString str;
	//str.clear();

	/*str="update device set configured = 0, b_present = "+QString::number(ui.cb_B->checkState()==Qt::Checked,10)+", "+
		"c_present = "+QString::number(ui.cb_C->checkState()==Qt::Checked,10)+", "+
		"k1_present = "+QString::number(ui.cb_K1->checkState()==Qt::Checked,10)+", "+
		"k2_present = "+QString::number(ui.cb_K2->checkState()==Qt::Checked,10)+", "+
		"door_present = "+QString::number(cb_door->checkState()==Qt::Checked,10)+" "+
		//"data_mode = "+QString::number(cb_datamode->checkState()==Qt::Checked,10)+", "+
		//"delta_1 = 0, delta_2 = 0, delta_3 = 0, delta_4 = 0 "+
		"where rec_id = "+QString::number(curIndex,10);
	if (!tq->exec(str))
	{
		QMessageBox::critical(0, qApp->tr("Ошибка отправки конфигурации"),
					tr("Ошибка"), QMessageBox::Cancel,
					QMessageBox::NoButton);
	}*/

	/*for (int i=0;i<9;i++)
	{
		tq->prepare("update shuno_config set "
			"type = :type, "
			"human_name = :human_name "
			"where device_id = "+QString::number(curIndex,10)+
			" and ckeck_point_number = "+QString::number(i+1,10)
			);
			tq->bindValue(":type",comboArray[i]->itemData(comboArray[i]->currentIndex()));
			tq->bindValue(":human_name",lineEditArray[i]->text());
			if (!tq->exec())
			{
				QMessageBox::critical(0, qApp->tr("Ошибка отправки конфигурации"),
					tr("Ошибка"), QMessageBox::Cancel,
					QMessageBox::NoButton);
			}
	}
	parent->master->updateDevice();
	parent->master->updateConfig();
	*/
	/*if(key_config)
		parent->setMyConfig(true);
	else
		parent->setMyConfig(false);*/

        //-----
        //0         1
        //1         B
        if (ui.cb_B->checkState()==Qt::Checked)
		{	mask|=0x40000;	}
        //2         C
        if (ui.cb_C->checkState()==Qt::Checked)
		{	mask|=0x20000;	}
        //3         F1
        if (checkArray[0]->checkState()==Qt::Checked)
		{	mask|=0x10000;	}
        //-----
        //4         1
        //5         1
        //6         1
        //7         0
        //-----
        //8         F2
        if (checkArray[1]->checkState()==Qt::Checked)
		{	mask|=0x00800;	}
        //9         F3
        if (checkArray[2]->checkState()==Qt::Checked)
		{	mask|=0x00400;	}
        //10        F4
        if (checkArray[3]->checkState()==Qt::Checked)
		{	mask|=0x00200;	}
        //11        F5
        if (checkArray[4]->checkState()==Qt::Checked)
		{	mask|=0x00100;	}
        //-----
        //12        F6
        if (checkArray[5]->checkState()==Qt::Checked)
		{	mask|=0x00080;	}
        //13        F7
        if (checkArray[6]->checkState()==Qt::Checked)
		{	mask|=0x00040;	}
        //14        F8
        if (checkArray[7]->checkState()==Qt::Checked)
		{	mask|=0x00020;	}
        //15        F9
        if (checkArray[8]->checkState()==Qt::Checked)
		{	mask|=0x00010;	}
        //-----
        //16        K1
        if (ui.cb_K1->checkState()==Qt::Checked)
		{	mask|=0x00008;	}
        //17        K2
        if (ui.cb_K2->checkState()==Qt::Checked)
		{	mask|=0x00004;	}
        //18        Door
        if (ui.cb_door->checkState()==Qt::Checked)
		{	mask|=0x00002;	}
        //19        0
        //-----

        QString str,msk;
        str=QString::number(mask,16);
        msk.prepend(str.at(4));
        msk.prepend(str.at(3));
        msk.prepend(str.at(2));
        msk.prepend(str.at(0));
		str.clear();
		str.append("CNP="+QString::number(parent->master->deviceDescr.counter_number,10)+
								","+QString::number(parent->master->deviceDescr.counter_pass,10)+
								","+QString::number((parent->master->deviceDescr.counter_type-1),10)+";");
        emit putCommand(curIndex,parent->master->deviceDescr.data_mode?str+"MSK="+msk+";":QString::number(mask,16),17+50*parent->master->deviceDescr.data_mode);

/*	if(!parent->master->deviceDescr.data_mode)
		//parent->setMyConfig(true);
	{
		emit ipconf_putCommand(curIndex,mask,17);
	}
	else
	{
		QSqlQuery* tq=new QSqlQuery();
		QString _COMM_STR,str,msk;
		QChar ch;
		int in_cmd=17;

		_COMM_STR.clear();

		str=QString::number(mask,16);
		msk.prepend(str.at(4));
		msk.prepend(str.at(3));
		msk.prepend(str.at(2));
		msk.prepend(str.at(0));

		_COMM_STR.append("MSK="+msk+";");
		parent->parent->COMM_STR.clear();
		parent->parent->COMM_STR.append(parent->master->deviceDescr.phone+"#"+_COMM_STR);
		if (parent->master->deviceDescr.counter_toSend&&(parent->master->deviceDescr.counter_type>0))//в terminalsedit стоит галочка на отправку && счётчик присутствует
		{
			_COMM_STR.clear();
			_COMM_STR.append("CNP="+QString::number(parent->master->deviceDescr.counter_number,10)+
								","+QString::number(parent->master->deviceDescr.counter_pass,10)+
								","+QString::number((parent->master->deviceDescr.counter_type-1),10)+";");
			parent->parent->COMM_STR.append(_COMM_STR);
		}
		str.clear();
		str=str+"values ("+QString::number(curIndex,10)+","+QString::number(in_cmd,10)+", "+"NOW(), "+"NOW(), "+"12, "+QString::number(in_cmd,10)+")";
		tq->exec("insert into send (device_id, command, time_seting, time_send, command_state, command_type) "+str);

		emit ipconf_signalAllToServer();
	}
	delete tq;*/
	hide();
}

void IPConfigurator::on_pbCancel_clicked()
{
	//emit refreshG();
	//tm_state->stop();
	hide();
}

void IPConfigurator::on_cb_B_stateChanged(int newState)
{
	b = (bool)newState;
	cp2 = b&&k1;
	cp5 = b&&k2;
	cp8 = b&&k2;
	ui.cb_2->setCheckState((cp2)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_5->setCheckState((cp5)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_8->setCheckState((cp8)?(Qt::Checked):(Qt::Unchecked));
	if (b){
		ui.cb_2->setEnabled(k1);
		ui.cb_5->setEnabled(k2);
		ui.cb_8->setEnabled(k2);
	}else{
		ui.cb_2->setEnabled(FALSE);
		ui.cb_5->setEnabled(FALSE);
		ui.cb_8->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_C_stateChanged(int newState)
{
	c = (bool)newState;
	cp3 = c&&k1;
	cp6 = c&&k2;
	cp9 = c&&k2;
	ui.cb_3->setCheckState((cp3)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_6->setCheckState((cp6)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_9->setCheckState((cp9)?(Qt::Checked):(Qt::Unchecked));
	if (c){
		ui.cb_3->setEnabled(k1);
		ui.cb_6->setEnabled(k2);
		ui.cb_9->setEnabled(k2);
	}else{
		ui.cb_3->setEnabled(FALSE);
		ui.cb_6->setEnabled(FALSE);
		ui.cb_9->setEnabled(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////



void IPConfigurator::on_cb_K1_stateChanged(int newState)
{
	k1 = (bool)newState;
	cp1 = k1;
	cp2 = k1&&b;
	cp3 = k1&&c;
	ui.cb_1->setCheckState((cp1)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_2->setCheckState((cp2)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_3->setCheckState((cp3)?(Qt::Checked):(Qt::Unchecked));
	if (k1){
		ui.cb_1->setEnabled(TRUE);
		ui.cb_2->setEnabled(b);
		ui.cb_3->setEnabled(c);
	}else{
		ui.cb_1->setEnabled(FALSE);
		ui.cb_2->setEnabled(FALSE);
		ui.cb_3->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_K2_stateChanged(int newState)
{
	k2 = (bool)newState;
	cp4 = k2;
	cp5 = k2&&b;
	cp6 = k2&&c;
	cp7 = k2;
	cp8 = k2&&b;
	cp9 = k2&&c;
	ui.cb_4->setCheckState((cp4)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_5->setCheckState((cp5)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_6->setCheckState((cp6)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_7->setCheckState((cp7)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_8->setCheckState((cp8)?(Qt::Checked):(Qt::Unchecked));
	ui.cb_9->setCheckState((cp9)?(Qt::Checked):(Qt::Unchecked));
	if (k2){
		ui.cb_4->setEnabled(TRUE);
		ui.cb_5->setEnabled(b);
		ui.cb_6->setEnabled(c);
		ui.cb_7->setEnabled(TRUE);
		ui.cb_8->setEnabled(b);
		ui.cb_9->setEnabled(c);
	}else{
		ui.cb_4->setEnabled(FALSE);
		ui.cb_5->setEnabled(FALSE);
		ui.cb_6->setEnabled(FALSE);
		ui.cb_7->setEnabled(FALSE);
		ui.cb_8->setEnabled(FALSE);
		ui.cb_9->setEnabled(FALSE);
	}
}


//////////////////////////////////////////////////////////////////////////



void IPConfigurator::on_cb_1_stateChanged(int newState)
{
	cp1 = (bool)newState;
	if (cp1){
		ui.combo_1->setEnabled(TRUE);
		ui.le_1->setEnabled(TRUE);
	}else{
		ui.combo_1->setEnabled(FALSE);
		ui.le_1->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_2_stateChanged(int newState)
{
	cp2 = (bool)newState;
	if (cp2){
		ui.combo_2->setEnabled(TRUE);
		ui.le_2->setEnabled(TRUE);
	}else{
		ui.combo_2->setEnabled(FALSE);
		ui.le_2->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_3_stateChanged(int newState)
{
	cp3 = (bool)newState;
	if (cp3){
		ui.combo_3->setEnabled(TRUE);
		ui.le_3->setEnabled(TRUE);
	}else{
		ui.combo_3->setEnabled(FALSE);
		ui.le_3->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_4_stateChanged(int newState)
{
	cp4 = (bool)newState;
	if (cp4){
		ui.combo_4->setEnabled(TRUE);
		ui.le_4->setEnabled(TRUE);
	}else{
		ui.combo_4->setEnabled(FALSE);
		ui.le_4->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_5_stateChanged(int newState)
{
	cp5 = (bool)newState;
	if (cp5){
		ui.combo_5->setEnabled(TRUE);
		ui.le_5->setEnabled(TRUE);
	}else{
		ui.combo_5->setEnabled(FALSE);
		ui.le_5->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_6_stateChanged(int newState)
{
	cp6 = (bool)newState;
	if (cp6){
		ui.combo_6->setEnabled(TRUE);
		ui.le_6->setEnabled(TRUE);
	}else{
		ui.combo_6->setEnabled(FALSE);
		ui.le_6->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_7_stateChanged(int newState)
{
	cp7 = (bool)newState;
	if (cp7){
		ui.combo_7->setEnabled(TRUE);
		ui.le_7->setEnabled(TRUE);
	}else{
		ui.combo_7->setEnabled(FALSE);
		ui.le_7->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_8_stateChanged(int newState)
{
	cp8 = (bool)newState;
	if (cp8){
		ui.combo_8->setEnabled(TRUE);
		ui.le_8->setEnabled(TRUE);
	}else{
		ui.combo_8->setEnabled(FALSE);
		ui.le_8->setEnabled(FALSE);
	}
}

void IPConfigurator::on_cb_9_stateChanged(int newState)
{
	cp9 = (bool)newState;
	if (cp9){
		ui.combo_9->setEnabled(TRUE);
		ui.le_9->setEnabled(TRUE);
	}else{
		ui.combo_9->setEnabled(FALSE);
		ui.le_9->setEnabled(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////
