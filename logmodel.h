#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QSqlQueryModel>
#include <QImage>

class LogModel : public QSqlQueryModel
{
	Q_OBJECT

public:
	LogModel(QObject *parent = 0);
	
	QVariant data(const QModelIndex &item, int role) const;
	void setCol();

private:
	QRgb rgb[15];
	
	
};

#endif // LOGMODEL_H
