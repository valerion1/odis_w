#ifndef DATATYPES_H
#define DATATYPES_H

#include <QDateTime>
#include <QList>

class IPObject;
class GroupWidget;

typedef struct _step{
		quint8 num;
		bool act;
		QTime time;
		quint8 knkv;
		int delta;
		bool primary;//
		bool changed;//было ли изменение со стороны диспетчера
	} _step;

//typedef _steps _step[8];

typedef struct Avtonom
{
	bool key[4];
	QString tm[4];
	//bool key2;
	//QString tm2;
	//bool key3;
	//QString tm3;
	//bool key4;
	//QString tm4;
	int state[4];//1,state2,state3,state4;
}Avtonom;

typedef struct Event_H
{
	quint8	Event;
	quint8	H;
	quint8	M;
}Event_H;

typedef struct Event_Table_H
{
	Event_H Event[12][31][4];
	QString	Name[32];	
	QString	Date[32];
}Event_Table_H;

struct SendGroupData
{
	QString datatimestring;
	QString deltastring;
	QString configstring;
	QString userstring;
};

struct GetGroupData
{
	QString statestring;
	QString datatimestring;
	QString deltastring;
	QString configstring;
};

#define counters_number 8//общее количество счётчиков

/*struct Count_password
{
	QString name;
	int number;
	int password;
};*/

typedef enum MonitorShowMode{
	MODE_GG,
	MODE_GA,
	MODE_STATE,
	MODE_COUNT,
	MODE_ALL
}MonitorShowMode;


struct DeviceDesr{
	QString name;
	QString phone;
	QString adress;
	int		dev_hard_number;
	int		group_g;
	int		group_a;
	int		avtonom_present;///!!! был bool, до того как стали хранить в нём состояние контакторов для дополнительных шагов
	//bool	counter_present;//в базе есть, в мониторе не используется, вместо него теперь counter_toSend
	bool	counter_toSend;
	int		counter_type;
	int		counter_number;
	int		counter_pass;
	int		counter_precision;
	bool	b_present;
	bool	c_present;
	bool	k1_present;
	bool	k2_present;
	bool	door_present;
	bool	is_error;
	int		is_avto;
	QString notes;
	float	x;
	float	y;
	QString counter_1_text;
	QString counter_2_text;
	int		counter_1_low_int;
	int		counter_2_low_int;
	int		counter_1_hi_int;
	int		counter_2_hi_int;
	bool	is_configured;
	bool	is_viewed;
	bool	is_crc;
	bool	is_root;
	int		position;
	int		delta_1;
	int		delta_2;
	int		delta_3;
	int		delta_4;
	int		delta_5;
	int		delta_6;
	int		delta_7;
	int		delta_8;
	int		delta_9;
	int		delta_10;
	int		delta_11;
	int		delta_12;
	int		delta_13;
	int		delta_14;
	int		delta_15;
	int		delta_16;
	QString counter_1_date;
	QString counter_2_date;
	bool    data_mode;
};

struct DeviceConfig{//описание ОДНОГО входа (всего 9)
	int ckeck_point_number;
	int k_number;
	int phase_number;
	QString human_name;
	int type;
	bool is_present;
	bool is_error;
	int last_state;
};

struct DeviceState{
	QDateTime time_stamp;
	int a_state;
	int b_state;
	int c_state;
	int k1_state;
	int k2_state;
	int kt1_state;
	int kt2_state;
	int kt3_state;
	int kt4_state;
	int kt5_state;
	int kt6_state;
	int kt7_state;
	int kt8_state;
	int kt9_state;
	int door_state;
};

typedef enum PhaseState{
	phase_state_UNCR,
	phase_state_NONE,
	phase_state_PRESENT,
	phase_state_ABSENT
};

typedef enum KState{
	k_state_UNCR,
	k_state_NONE,
	k_state_ON_OK,
	k_state_ON_FAIL,
	k_state_OFF_OK,
	k_state_OFF_FAIL
};
typedef enum FState{
	f_state_UNCR,
	f_state_NONE,
	f_state_ON_OK,
	f_state_ON_FAIL,
	f_state_OFF_OK,
	f_state_OFF_FAIL
};
typedef enum FBState{
	fb_state_UNCR,
	fb_state_NONE,
	fb_state_ON_OK,
	fb_state_ON_FAIL,
	fb_state_OFF_OK,
	fb_state_OFF_FAIL
};
typedef enum DState{
	d_state_UNCR,
	d_state_NONE,
	d_state_CLOSED,
	d_state_OPENED
};

typedef QList<IPObject *> DevList;

struct GrList{
	int index;
	QString name;
	QString descr;
	int devCount;
	DevList deviceList;
	GroupWidget *gw;
};

//Иконка ШУНО - Ручной режим
#define colorSHUNO_Manual							0
//Иконка ШУНО - Автоматический режим
#define colorSHUNO_Auto								1
//Иконка ШУНО - Рамка аварии
#define colorSHUNO_Error							2
//Иконка ШУНО - Ошибка приема
#define colorSHUNO_ErrorCRC							3
//Иконка ШУНО - Ошибка автонома
#define colorSHUNO_ErrorPAR							4
//Иконка ШУНО - Контактор ВЫКЛ
#define colorSHUNO_KOff								5
//Иконка ШУНО - Контактор ВКЛ
#define colorSHUNO_KOn								6
//Фаза А
#define colorPhaseA									7
//Фаза В
#define colorPhaseB									8
//Фаза С
#define colorPhaseC									9
//Журнал - Отправленная команда
#define colorLog_Send								10			
//Журнал - Прием неаварийной квитанции состояния
#define colorLog_RecvStateOk						11					
//Журнал - Прием аварийной квитанции состояния
#define colorLog_RecvStateError						12					
//Журнал - Прием правильного времени
#define colorLog_RecvTimeOk							13				
//Журнал - Прием ошибочного времени
#define colorLog_RecvTimeError						14					
//Журнал - Прием данных счетчика
#define colorLog_RecvCounterOk						15					
//Журнал - Прием данных счетчика с ошибкой
#define colorLog_RecvCounterError					16						
//Журнал - Ошибка приема
#define colorLog_RecvCRC							17							
//
//


const int DeltaArray[16]={0,-5,-10,-15,-25,-35,-45,-55,-61,5,10,15,25,35,50,61};

/*struct GroupContainer{
	
};*/

#endif // DATATYPES_H