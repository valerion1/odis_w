#include "devicebutton.h"
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QSound>
#include <QMouseEvent>
#include "basesettings.h"
#include "ipwindow.h"


DeviceButton::DeviceButton(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

DeviceButton::~DeviceButton()
{

}

void DeviceButton::setFace(QImage *in_image){
    face = *in_image;
    ui.label_2->setPixmap(QPixmap::fromImage(face));
}

void DeviceButton::setName(QString in_name){
    ui.label->setText(in_name);
}

void DeviceButton::setDescr(QString in_hint){
    ui.label_2->setToolTip(in_hint);
}

void DeviceButton::setIndex(IPObject *in_master){
    master = in_master;
    index = master->rec_id;
}

void DeviceButton::setDetails(QImage *in_details){
    details = in_details;
}

void DeviceButton::mousePressEvent(QMouseEvent * event){
    if ((event->x() > 0)&&(event->x() < 63)){
        if ((event->y() > 0)&&(event->y() < 63)){
            //BaseSettings *bs = BaseSettings::createInstance();
            master->updateDevice();
            master->updateConfig();
            master->updateState();
            master->updateImages();
            emit deviceClicked(master);//переставить выше, попробовать открыть окно до отрисовки history
            //monitor::ipW->iShow(master);
            //bs->freeInst();
            if (event->button() == Qt::LeftButton) {    QPoint lastPoint = event->pos();    }
        }
    }
}

/*
void DeviceButton::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawImage(2,2,face);
}
*/
