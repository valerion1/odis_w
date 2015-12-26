#include "logmodel.h"
#include "basesettings.h"

#include <QtGui>

LogModel::LogModel(QObject *parent)
	: QSqlQueryModel(parent)
{
	
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
	QVariant value = QSqlQueryModel::data(index, role);
// 	if (value.isValid() && role == Qt::DisplayRole) {
// 		if (index.column() == 0)
// 			return value.toString().prepend("#");
// 		else if (index.column() == 2)
// 			return value.toString().toUpper();
// 	}
	if (role == Qt::BackgroundRole ){//&& index.column() == 0){
		if (this->index(index.row(),4).data().toInt()==1){
			return qVariantFromValue(QColor(rgb[9]));
		}else{
			switch (this->index(index.row(),0).data().toInt()){
				case 1:
					//1	������������
					return qVariantFromValue(QColor(QRgb(rgb[7])));
					break;
				case 2:
					//2	��������� ����
					return qVariantFromValue(QColor(rgb[8]));
					break;
				case 3:
					//3	����� ����
					return qVariantFromValue(QColor(Qt::green));
					break;
				case 4:
					//4 �������
					return qVariantFromValue(QColor(rgb[12]));
					break;
				case 5:
					//5 ����� ����
					return qVariantFromValue(QColor(rgb[10]));
					break;
				case 6:
					//6 ������ ������
					return qVariantFromValue(QColor(rgb[14]));
					break;
				default:
					//	����
					return qVariantFromValue(QColor(Qt::white));
					break;
			}
		}
	}
	return value;
}
void LogModel::setCol()
{
	BaseSettings *bs = BaseSettings::createInstance();
	int colorsCount = bs->settings->value("colors/col_count","0").toInt();
	for (int i=0;i<colorsCount;i++)
	{
		rgb[i]=bs->settings->value("colors/color_"+QString::number(i,10)+"_value","0").toUInt();
	}
	bs->freeInst();
}