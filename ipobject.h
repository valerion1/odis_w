#ifndef IPOBJECT_H
#define IPOBJECT_H
#include "datatypes.h"
#include "monitor.h"
#include <QObject>
#include <QIcon>
#include <QtWidgets/QListWidgetItem>
#include <QImage>

class DeviceButton;
class monitor;

class IPObject : public QObject
{
	Q_OBJECT

public:
	IPObject(monitor *parent, int in_index);
	~IPObject();
	void loadDevice();//первична€ загрузка приборов, обновл€етс€ всЄ из базы
	void updateDevice();//наверное стоит это убрать по возможности, пока что убрано перечитывание параметров счЄтчиков
	void updateConfig();
	void updateState();
	void updateMainIcon();
	void saveData();
	void updateImages();
	void setCol();
	int getGroupG();
	int getGroupA();
	int getGroupS();
	DeviceButton* getButton();
	int rec_id;
	DeviceDesr deviceDescr;
	DeviceConfig deviceConfig[9];
	DeviceState deviceState;
	QImage *monitorIcon;
	QImage *details;
	int lavel;
	bool key_anim;
	bool bitstate(int inData,int pos);

//signals:
	//void deviceToIPW(IPObject * master);

/*public slots:
	void getDeviceClick(IPObject * master);
*/

private:
	bool newIncoming;
	//QListWidgetItem *deviceIcon;
	DeviceButton *devButton;
	QRgb rgb[15];
	bool bpf_state;
	int bpf_ph_kn,bpf_ph_kv;
	int mess_state;
	bool compareDevices(const IPObject *,const IPObject *);
	monitor * parent;
};

#endif // IPOBJECT_H
//7874350
