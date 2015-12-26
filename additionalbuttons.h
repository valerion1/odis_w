#ifndef ADDITIONALBUTTONS_H
#define ADDITIONALBUTTONS_H

#include <QtWidgets/QWidget>
// old #include <QWidget>
#include <QTimer>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QComboBox>
#include "ui_additionalbuttons.h"
#include "ipwindow.h"

class IPWindow;
class PARButtons;
class CCheckBox;
class CPushButton;
class CTimeEdit;
class CComboBox;


class AdditionalButtons : public QWidget
{
	Q_OBJECT

public:
	Ui::AdditionalButtonsClass ui;
	bool key_login;
	bool key_anycommand;
	AdditionalButtons(QWidget *parent = 0);
	~AdditionalButtons();
	IPWindow	*parent;
	void setParent(IPWindow *parent);
	monitor *parent2;
	void setParent2(monitor *parent2);

	CCheckBox	*par_act[8];
	//PARButtons*par_buttons[8];
	QLabel		*par_event[8];
	CTimeEdit	*par_time[8];
	CPushButton	*par_kn[8];
	CPushButton	*par_kv[8];
	CComboBox	*par_delta[8];
	//QLabel	*debugLabel;
	_step stepsADD[8];//массив шагов для сортировки и вывода в отображении шагов + дельт

	void toDebug(QString _text);
	//int getVoiceDeltaCommand(void);
	QString getDeltaString(bool data_mode);//,_step * in_steps
	//void setPrimaryDelta(int num_step,int delta);

private:
	//Ui::AdditionalButtonsClass ui;
	//int devID;
	IPObject * device_ptr;//одно устройство для работы табличек
	DevList * devList;//список устройств для групповой отправки
	int DeltaArr[16];
	//_step group_steps[8];//массив шагов для сортировки и вывода в отображении шагов + дельт

	int hour,min;
	void keyPressEvent(QKeyEvent *event);
	void redrawParGrid(IPObject *in_device);
	void refreshGroupParSteps(_step * group_steps);
	static int sort_stepsADD(const void * p1,const void * p2);	//функция сортировки двумерного массива по шагам
	static int resort_stepsADD_toSend(const void * p1,const void * p2);	//функция сортировки двумерного массива по шагам для отправки

public slots:
	//void IShow(int in_devID);
	void IShow(IPObject * in_device_ptr);
	void groupShow(DevList * deviceList);//на первом в группе устанавливаем значения, потом в цикле передаём
	void setAccessLevel();
	void changeAdditionalStep(int state,int row,bool _active);
	void knkvToggled(bool _checked,int _row,bool _knkv);//_step * in_steps);//обновляет состояние контакторов для дополнительного шага
	void timeChange(int _delta,int _row);//обновляет дельту в массиве для основного шага
	void updateSecondaryDelta(QTime _time, int _row);//обновляет дельту в массиве для дополнительного шага
	//void changeAdditionalStepAct(int state,int row);

private slots:
	void on_pbClose_clicked();
	void on_pbGPRS_Stat_clicked();
	void on_pbGPRS_Avtonom_clicked();
	void on_pbGPRS_Soft_clicked();
	void on_pbDeltaSend_clicked();//отправить дельту на один прибор и закрыть окно
	void on_pbDeltaSendToGroup_clicked();//отправить дельту на группу приборов и закрыть окно
	void on_pbRebootHard_clicked();
	void on_pbRebootSoft_clicked();
	//void tm_stateTimer();
/*	void on_delta13_checkBox_stateChanged(int state);	//включается возможность редактировать дополнительные шаги
	void on_delta14_checkBox_stateChanged(int state);
	void on_delta15_checkBox_stateChanged(int state);
	void on_delta16_checkBox_stateChanged(int state);
	void on_delta13_KH_pushButton_clicked(bool state);	//меняется иконка на кнопке для дополнительных шагов
	void on_delta13_KB_pushButton_clicked(bool state);
	void on_delta14_KH_pushButton_clicked(bool state);	//меняется иконка на кнопке для дополнительных шагов
	void on_delta14_KB_pushButton_clicked(bool state);
	void on_delta15_KH_pushButton_clicked(bool state);	//меняется иконка на кнопке для дополнительных шагов
	void on_delta15_KB_pushButton_clicked(bool state);
	void on_delta16_KH_pushButton_clicked(bool state);	//меняется иконка на кнопке для дополнительных шагов
	void on_delta16_KB_pushButton_clicked(bool state);
	void on_cBoxDelta_1_editTextChanged(QString delta);	//при изменении дельты сразу пересчитывать время
	void on_cBoxDelta_2_editTextChanged(QString delta);	//при изменении дельты сразу пересчитывать время
	void on_cBoxDelta_3_editTextChanged(QString delta);	//при изменении дельты сразу пересчитывать время
	void on_cBoxDelta_4_editTextChanged(QString delta);	//при изменении дельты сразу пересчитывать время
	qint8 check_delta_intersection(quint8 step,int delta);
*/
	
	//bool eventFilter(QObject *object, QEvent *event);	//при изменении дельты сразу пересчитывать время
	//void on_par_act_stateChanged(int state,int row);
	//void deltaCheckBoxStateChanged(int state);

signals:
	//void emitCmd(int dev_id, int cmd, int type);
    void putCommand(int device, QString command, int command_type);
	//void reloadavt();
};


/*class PARButtons: public QWidget
{
    Q_OBJECT
public:
	QLayout * knkv_layout;
	QPushButton * kn;
	QPushButton * kv;
    PARButtons(AdditionalButtons * parent,int _grid_y,int _grid_x,quint8 _knkv);//конструктор
	~PARButtons();//деструктор
	void setKNKVEnabled(bool _mode);//сделать активными/неактивными кнопки
};
*/

class CCheckBox: public QCheckBox
{
	Q_OBJECT
	public:
		CCheckBox(QWidget *parent = 0,int _row=0, int _step_num=0);
	private:
		int state;
		int row;
		int step_number;

	private slots:
		void ccboxStateChanged(int _state);
		//void pasStateChanged(int _state);
		//void actStateChanged(int _state);
	signals:
		void cboxStateChanged(int _state, int _row, bool _active);
};

class CPushButton: public QPushButton
{
	Q_OBJECT
	public:
		bool checked;
		bool kn;
		CPushButton(QWidget *parent = 0,int _row=0, int _step_num=0, bool _kn=0);
		int num_step(void){return this->step_number;}
	private:
		int row;
		int step_number;
	private slots:
		void knChanged(bool _checked);
		void kvChanged(bool _checked);
	signals:
		void customToggled(bool _checked, int _row, bool _knkv);
};

class CTimeEdit: public QTimeEdit
{
	Q_OBJECT
	public:
		QTime rawtime;//время без дельты
		CTimeEdit(QWidget *parent=0,int _row=0, int _step_num=0, QTime _time=QTime(0,0,0,0));
        int num_step(void){
            return step_number;
        }
	private:
		int row;
		int step_number;
	private slots:
		void updateDelta(QTime _time); //передаёт номер шага и значение в updateDeltas
	signals:
		void secDeltaChanged(QTime _time,int _row);//сигнал в AdditionalButtons::updateDeltas()
};

class CComboBox: public QComboBox
{
	Q_OBJECT
	public:
		CComboBox(QWidget *parent=0,int _row=0, int _step_num=0);
	private:
		int row;
		int step_number;
	private slots:
		void deltaChanged(QString _textdelta);
	signals:
		void changeTime(int _delta,int _row);
};


#endif // ADDITIONALBUTTONS_H
