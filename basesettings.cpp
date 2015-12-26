#include "basesettings.h"
#include "ipwindow.h"

#include <QSettings>
#include <QSqlDatabase>
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>

BaseSettings::BaseSettings()
	: QObject(NULL)
{
	settings = new QSettings("monitor.ini", QSettings::IniFormat,this);
	settings->setIniCodec("Windows-1251");
	connectedToApp = FALSE;
	errFile = new QFile();
}

BaseSettings::~BaseSettings()
{
	delete settings;
}


QSqlQuery* BaseSettings::safeExec(QString queryText, QString module){
	
	/*
	errFile->setFileName("system_"+QDateTime::currentDateTime().toString("yyyy.MM.dd")+".txt");
	if (errFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
		QTextStream out(errFile);
		out << QDateTime::currentDateTime().toString("yyyy.MM.dd - hh:mm:ss     ")
			<< sourceFile 
			<< ", line "
			<< line
			<< " - "
			<< describe
			<< "\n";
		out.flush();
		errFile->flush();
		errFile->close();
	}
*/
	return NULL;
}