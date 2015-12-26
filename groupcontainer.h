#ifndef GROUPCONTAINER_H
#define GROUPCONTAINER_H

#include <QObject>
#include "ipobject.h"
#include "groupwidget.h"

class GroupContainer : public QObject
{
	Q_OBJECT

public:
	GroupContainer(QObject *parent, int in_index, QString in_name);
	~GroupContainer();

private:
	QList<IPObject*> deviceList;
	QString name;
	int groupID;
	GroupWidget *g;
};

#endif // GROUPCONTAINER_H
