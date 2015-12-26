#ifndef BASESETTINGS_H
#define BASESETTINGS_H

#include <QObject>
#include "singleton.h"
#include <QSettings>
#include <QFile>
#include "datatypes.h"
#include "groupcontainer.h"
#include <QSqlQuery>

class QSqlDatabase;

class BaseSettings : public QObject, public Singleton<BaseSettings>
{
	Q_OBJECT

protected:
	BaseSettings();
	~BaseSettings();
	friend class Singleton<BaseSettings>;

public:
	QSettings *settings;

	bool connectedToApp;
	static QSqlQuery* safeExec(QString queryText, QString module);
	int accessLevel;
	int colors[18];

private:
	QFile *errFile;
};

#endif // BASESETTINGS_H
