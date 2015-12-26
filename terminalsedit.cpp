#include "terminalsedit.h"
#include "ipconfigurator.h"
#include "ipobject.h"
#include "basesettings.h"
#include "pr_excel.h"

#include <QTextStream>
#include <QFile>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QtWidgets/QMessageBox>
#include <QModelIndex>
#include <QtWidgets/QHeaderView>
#include <QSqlField>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QSqlRecord>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QSqlQueryModel>
#include <QPicture>
#include <QtWidgets/QFileDialog>
#include <QImage>
#include <QValidator>
#include <QRegExp>

TerminalsEdit::TerminalsEdit(QWidget *parent, DevList *in_deviceList)
    : QWidget(parent)
{
    ui.setupUi(this);
    currRec = -1;//номер прибора в базе, с которым работаем в данный момент, в любой момент времени должен быть адекватным
    //terminal_cursor_position = ui.tableWidget->model()->index(-1, 1);
    terminal_cursor_position_int = -1;

    deviceList=in_deviceList;
    refrEnable = FALSE;//мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?) (!!!убрать!!!)
    ipConf = new IPConfigurator(NULL);
    ui.le_phone->setMaxLength(13);
    //connect(ui.tableWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(on_t b_clicked(QModelIndex)));
    connect(ui.tableWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclicked(QModelIndex)));
    connect(ui.le_name, SIGNAL(textChanged(const QString &)), this, SLOT(nameTextChanged(const QString &)));
    key_login=false;
    position_view=0;
    position_key=1;
    fnd_terminal=NULL;
    fnd_terminal=new Find_terminal(NULL);
    key_prop=true;//ещё один мутный ключ, пока не ясно что делает (!!!убрать!!!)
    ptr=NULL;
    
    ui.pb_print->setText(tr("Печать"));
    pb_print=NULL;
    
    //pb_print=new QPushButton(tr("Печать"));
    //pb_print->setEnabled(true);
    //horizontalLayout=ui.hoframe->layout();
    //ui.horizontalLayout->addWidget(pb_print);
    //connect(pb_print,SIGNAL(clicked()),this,SLOT(on_pb_print_clicked()));
    
        
}

TerminalsEdit::~TerminalsEdit()
{
    delete ipConf;
    if(fnd_terminal)
        delete fnd_terminal;
    if(pb_print) delete pb_print;
}

void TerminalsEdit::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        hide();
    }
}

void TerminalsEdit::iShow()
{
    BaseSettings *settings = BaseSettings::createInstance();
    //mainBs = BaseSettings::createInstance();

    //ui.cb_datamode->setText(tr("Режим DATA"));
    ui.le_phone->setMaxLength(13);
    ui.le_dev_hard->setMaxLength(6);
    //заполнить комбо-боксы
    refrEnable = FALSE;//мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?) (!!!убрать!!!)
    ui.combo_group_g->clear();
    ui.combo_group_a->clear();
    QSqlQuery *tq = new QSqlQuery();
    if (!tq->exec("select rec_id, name from groups_g")){
        //settings->printError("terminalsedit.cpp",53,"error after <tq->exec(\"select rec_id, name from groups_g\")>")
        tq = BaseSettings::safeExec("select rec_id, name from groups_g","wqwer");
    }
    while (tq->next())
    {
        ui.combo_group_g->addItem(tq->value(1).toString(),tq->value(0).toInt());
    }
    tq->exec("select rec_id, name from avtonom_groups");
    while (tq->next())
    {
        ui.combo_group_a->addItem(tq->value(1).toString(),tq->value(0).toInt());
    }
    ui.comboBox_counter_precision->clear();//место точки в показаниях
    for (int j=0;j<=5;j++)
    {
        ui.comboBox_counter_precision->addItem(QString::number(j,10),j);
    }

    ui.comboBox_counter_type->clear();//тип счётчика

    int temp_counter_count=settings->settings->beginReadArray("counters");
    QStringList temp_counter_list;//=settings->settings->childGroups();//имя группы
    temp_counter_list.clear();
    if (temp_counter_count<=0)//если вдруг пусто, задаём массив по умолчанию
    {
        settings->settings->endArray();
        temp_counter_list=parent->counter_list_default;
        temp_counter_count=temp_counter_list.size();
    }
    else
    {
        for (int i=0;i<temp_counter_count;i++)
        {
            settings->settings->setArrayIndex(i);
            temp_counter_list.append(settings->settings->value("name").toString());
        }
        settings->settings->endArray();
    }
    for (int i=0;i<temp_counter_count;i++)
    {
        ui.comboBox_counter_type->addItem(temp_counter_list.at(i));
    }
    ui.lineEdit_counter_number->clear();
    ui.lineEdit_counter_pass->clear();
    
    delete tq;
    
    //////////////////////////////////////////////////////////////////////////
    ///int i = 0;
    //QString tStr;
    //int highlitedIndex = 1;
    ui.tableWidget->setSortingEnabled(true);
    /*while (tq->next())
    {
        tBoolVal = TRUE;
        tStr = tq->value(0).toString();
        if (cur rRec == 0) cu rrRec = tq->value(0).toInt();
        if (tq->value(0).toInt() == curr Rec){
            highlitedIndex = i;
        }
        //INDEX
        tItem = new QTableWidgetItem(tStr);
        tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui.tableWidget->setItem(i,0,tItem);
        //name
        tStr = tq->value(1).toString();
        tItem = new QTableWidgetItem(tStr);
        tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui.tableWidget->setItem(i,1,tItem);
        //phone
        tStr = tq->value(2).toString();
        tItem = new QTableWidgetItem(tStr);
        tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui.tableWidget->setItem(i,2,tItem);
        //hard_number
        tStr = tq->value(3).toString();
        tItem = new QTableWidgetItem(tStr);
        tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui.tableWidget->setItem(i,3,tItem);
        i++;
    }
    */
    //new fill
    int devCount=deviceList->size();
    //выбрать и отрисовать список девайсов
    //tq->exec("select rec_id, name, phone, dev_hard_number from device");
    ui.tableWidget->clear();
    QTableWidgetItem *tItem;
    
    ui.tableWidget->setColumnCount(5);
    ui.tableWidget->setRowCount(devCount);
    tItem = new QTableWidgetItem(tr("Id"));
    ui.tableWidget->setHorizontalHeaderItem(0,tItem);
    tItem = new QTableWidgetItem(tr("Имя"));
    ui.tableWidget->setHorizontalHeaderItem(1,tItem);
    tItem = new QTableWidgetItem(tr("Телефон"));
    ui.tableWidget->setHorizontalHeaderItem(2,tItem);
    tItem = new QTableWidgetItem(tr("Инв."));
    ui.tableWidget->setHorizontalHeaderItem(3,tItem);
    tItem = new QTableWidgetItem(tr("Индекс."));
    ui.tableWidget->setHorizontalHeaderItem(4,tItem);
    terminal_cursor_position_int=0;//по умолчанию курсор на 0 позиции
    for (int i=0;i<devCount;i++)
    {
        if (currRec < 0) currRec = deviceList->at(i)->rec_id;//начальные условия
        if (deviceList->at(i)->rec_id==currRec) terminal_cursor_position_int=i;//начальные условия
        /*if (deviceList->at(i)->rec_id == currRec)
        {   highlitedIndex = i; }*/
        //INDEX
        tItem = new QTableWidgetItem(QString::number(deviceList->at(i)->rec_id,10));
        //tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui.tableWidget->setItem(i,0,tItem);
        ui.tableWidget->setColumnHidden(0,true);
        //name
        tItem = new QTableWidgetItem(deviceList->at(i)->deviceDescr.name);
        //tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui.tableWidget->setItem(i,1,tItem);
        //phone
        tItem = new QTableWidgetItem(deviceList->at(i)->deviceDescr.phone);
        //tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui.tableWidget->setItem(i,2,tItem);
        //hard_number
        tItem = new QTableWidgetItem(QString::number(deviceList->at(i)->deviceDescr.dev_hard_number));
        //tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui.tableWidget->setItem(i,3,tItem);
        //index
        tItem = new QTableWidgetItem(QString::number(deviceList->at(i)->deviceDescr.position));
        //tItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        tItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui.tableWidget->setItem(i,4,tItem);
        ui.tableWidget->setColumnHidden(4,true);
        //counters
        ui.comboBox_counter_type->setCurrentIndex(deviceList->at(i)->deviceDescr.counter_type);//deviceDescr.counter_type=0 - нет счётчика; 1,2,... - по списку в терминале, со сдвигом на -1
        ui.lineEdit_counter_number->setText(QString::number(deviceList->at(i)->deviceDescr.counter_number));
        ui.lineEdit_counter_pass->setText(tr("-1"));
        ui.comboBox_counter_precision->setCurrentIndex(deviceList->at(i)->deviceDescr.counter_precision);
    }

    if (devCount>0)//установим курсор на поле в таблице и включим кнопку удаления
    {
        ui.tableWidget->selectRow(terminal_cursor_position_int);
        if (key_login) ui.pbDelDevice->setDisabled(false);//пока курсор не стоит на нормальной записи, удалять ничего не разрешается
    }
    else ui.pbDelDevice->setDisabled(true);//пока курсор не стоит на нормальной записи, удалять ничего не разрешается

    //resizing
    ui.tableWidget->resizeColumnsToContents();
    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    // old ui.tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    /*int totalWidth = ui.tableWidget->width();
    totalWidth -= ui.tableWidget->columnWidth(0);
    totalWidth -= ui.tableWidget->columnWidth(2);
    totalWidth -= ui.tableWidget->columnWidth(3);*/
    //ui.tableWidget->setColumnWidth(1,totalWidth);//-50);
    //ui.tableWidget->setCurrentCell(highlitedIndex,0);
    //ui.tableWidget->setSortingEnabled(TRUE);
    //ui.tableWidget->sortItems(1,Qt::AscendingOrder);
    //ui.tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    //ui.tableWidget->setAutoScroll(true);
    BDrefresh(currRec);
    refrEnable = TRUE;//мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?) (!!!убрать!!!)
    //установить курсор на первый девайс
    //отобразить параметры согласно выбранному девайсу
    //ui.cb_door_present->hide();

    ui.le_name->setMaxLength(50);
    //ui.le_name->installEventFilter(this);

    //ввод только цифр
    QRegExp rx( "^[0-9]*[+]{1}[0-9]*$" );
    QValidator *validator = new QRegExpValidator(rx, this);
    ui.le_phone->setValidator(validator);

    show();
}

void TerminalsEdit::on_comboBox_counter_present_stateChanged(int newState)
{
    ui.comboBox_counter_precision->setDisabled(!(bool)newState);
    ui.lineEdit_counter_number->setDisabled(!(bool)newState);
    ui.lineEdit_counter_pass->setDisabled(!(bool)newState);
    ui.comboBox_counter_type->setDisabled(!(bool)newState);
}

bool TerminalsEdit::validator(){
    return TRUE;
}

void TerminalsEdit::on_pbSave_clicked()
{
    QString str_name;
    QString str_phone;
    int int_dev_hard_number;
    QStringList list;
    int buf[4];
    int rez=0;
    bool key=false;
    bool already_in_base=false;

    str_phone=ui.le_phone->text();
    int_dev_hard_number=ui.le_dev_hard->text().toInt();

    str_name.clear();
    str_name=ui.le_name->text();
    str_name=str_name.trimmed();
    list =str_name.split(" ");
    str_name=list.at(0).trimmed()+" ";
    for(int i=1;i<list.size();i++)
    {
        if(!list.at(i).isEmpty())
            str_name=str_name+list.at(i).trimmed()+" ";
    }
    str_name=str_name.trimmed();

    if(key_login)
        ui.pbDelDevice->setDisabled(true);
    if(!ui.le_name->text().isEmpty())
    {
        if(key_prop)//ещё один мутный ключ, пока не ясно что делает (!!!убрать!!!)
        {
            /*if(str.size()==13 && str.at(0)=='+')
            {
                for(int y=1;y<str.size();y++)
                {
                    rez=str.at(y).unicode();
                    if(rez-48>=0 && rez-48<=9)
                    {
                        key=true;
                    }
                    else
                    {
                        key=false;
                        break;
                    }
                }
            }
            else
                key=false;
            if(!key)
                    QMessageBox::critical(0, qApp->tr("Ошибка"),tr("      Неверно набран номер!\nФормат номера +380XXXXXXXXX"),
                        QMessageBox::Ok,QMessageBox::NoButton);
            else
            {*/
                for(int i=0;i<deviceList->size();i++)//ищет совпадения по телефону или имени
                {
                    if(deviceList->at(i)->deviceDescr.name==str_name || deviceList->at(i)->deviceDescr.phone==str_phone || deviceList->at(i)->deviceDescr.dev_hard_number==int_dev_hard_number)
                    {
                        already_in_base=true;
                        break;
                    }
                }
            //}
        }
        /*else QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),
                tr("Просьба сообщить об этой ошибке разработчику с описанием действий и доп. информации, вызвавших это сообщение. Дополнительная информация: key_prop=false"), QMessageBox::Cancel,
                QMessageBox::NoButton);*///(!!!разобраться что такое key_prop)

        if (refrEnable && !already_in_base)//refrEnable мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?)  (!!!убрать!!!)
        {
            if (validator()) //validator!!!
            {
                ////////////проверка номера телефона////////////////////
                /*if(str.size()==13 && str.at(0)=='+')
                {
                    for(int y=1;y<str.size();y++)
                    {
                        rez=str.at(y).unicode();
                        if(rez-48>=0 && rez-48<=9)
                        {
                            key=true;
                        }
                        else
                        {
                            key=false;
                            break;
                        }
                    }
                }
                else
                    key=false;
                if(!key)
                        QMessageBox::critical(0, qApp->tr("Ошибка"),tr("      Неверно набран номер!\nФормат номера +380XXXXXXXXX"),
                            QMessageBox::Ok,QMessageBox::NoButton);
                else
                {*/
                    ///////////////////////////////////////////////////////
                    ////////ЁПРСТ
                    QSqlQuery *tq = new QSqlQuery();
                    tq->prepare("update device set "
                        "name = :name, "
                        "phone = :phone, "
                        "adress = :adress, "
                        "dev_hard_number = :dev_hard_number, "
                        "group_g = :group_g, "
                        "group_a = :group_a, "
                        //"avtonom_present = :avtonom_present, "//уже не используется
                        //"counter_present = :counter_present, "//уже не используется, вместо него counter_toSend, сохраняющийся только на время запуска монитора
                        "data_mode = :data_mode, "
                        "counter_precision = :counter_precision, "//записывается в спруте при успешной отправке квитанции с CNP=x,y,z;
                        //"counter_number = :counter_number, "//записывается в спруте при успешной отправке квитанции с CNP=x,y,z;
                        //"counter_type = :counter_type, "//записывается в спруте при успешной отправке квитанции с CNP=x,y,z;
                        //"door_present = :door_present, "//запись двери пересена в спрут при отправке CFG=xxxx;
                        "notes = :notes, "
                        "x = :x, "
                        "y = :y, "
                        "photo = :photo, "
                        "is_root = :is_root "
                        "where rec_id = "+QString::number(currRec,10)
                        );
                    tq->bindValue(":name",str_name);
                    tq->bindValue(":phone",ui.le_phone->text());
                    tq->bindValue(":adress",ui.le_adres->text());
                    tq->bindValue(":dev_hard_number",ui.le_dev_hard->text().toInt());
                    tq->bindValue(":group_g",ui.combo_group_g->itemData(ui.combo_group_g->currentIndex(),Qt::UserRole).toInt());
                    tq->bindValue(":group_a",ui.combo_group_a->itemData(ui.combo_group_a->currentIndex(),Qt::UserRole).toInt());
                    
                    if (ui.cbRoot->checkState() == Qt::Checked)
                    {
                        tq->bindValue(":is_root",TRUE); 
                        buf[1]=1;
                    }
                    else
                    {
                        tq->bindValue(":is_root",FALSE);    
                        buf[1]=0;
                    }
                    //-----------
                    bool terminal_mode=false;
                    if (ui.cb_datamode->checkState() == Qt::Checked)
                        terminal_mode=true;//это нормально? переделать по-человечески
                    tq->bindValue(":data_mode",terminal_mode);  
                    tq->bindValue(":counter_precision",ui.comboBox_counter_precision->currentIndex());
                    tq->bindValue(":notes",ui.te_notes->toPlainText());
                    tq->bindValue(":x",ui.le_x->text().toFloat());
                    tq->bindValue(":y",ui.le_y->text().toFloat());
                    //tq->bindValue(":photo",ui.photo_label->pixmap());
                    if (ui.photo_label->pixmap())
                        {
                        QPixmap pM(*ui.photo_label->pixmap());
                        tq->bindValue(":photo",QVariant(pM));
                        //tq->bindValue(":photo",0);
                        ui.photo_label->setPixmap(pM);
                        }
                    else
                        {tq->bindValue(":photo",0);}//photottotototo.....

                    //QString debugStr = tq->lastQuery();
                   /* QString s,s1,s2;
                    s1=s.setNum(ui.le_x->text().toFloat());
                    s.clear();
                    s2=s.setNum(ui.le_y->text().toFloat());
                    QString str="update device set name ='"+ui.le_name->text()+"', phone ='"+ui.le_phone->text()
                        +"', adress ='"+ ui.le_adres->text()+"', dev_hard_number ="+QString::number(ui.le_dev_hard->text().toInt(),10)
                        +", group_g ="+QString::number(ui.combo_group_g->itemData(ui.combo_group_g->currentIndex(),Qt::UserRole).toInt(),10)
                        +", group_a ="+QString::number(ui.combo_group_a->itemData(ui.combo_group_a->currentIndex(),Qt::UserRole).toInt(),10)
                        +", avtonom_present ="+QString::number(buf[0],10)+", counter_present ="+QString::number(buf[2],10)+", counter_precision ="
                        +QString::number(ui.combo_counter_precision->currentIndex(),10)+", door_present ="+QString::number(buf[3],10)+", notes ='"+ui.te_notes->toPlainText()+"', x ="
                        +s1+", y="+s2+", photo =0, is_root ="+QString::number(buf[1],10)+" where rec_id = "+QString::number(currRec,10);*/
                    if (!tq->exec())
                    {
                        QSqlError err = tq->lastError();
                        QMessageBox::critical(0, qApp->tr("Ошибка сохранения"),
                            err.text().toUtf8().constData()/*tr("Неверные введенные данные")*/,QMessageBox::Cancel,
                            //old err.text().toAscii().constData()/*tr("Неверные введенные данные")*/, QMessageBox::Cancel,
                            QMessageBox::NoButton);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    int devListLen = deviceList->size();
                    int currDevListIndex=-1;
                    for (int i=0;i<devListLen;i++)
                    {
                        if (deviceList->at(i)->rec_id == currRec)
                        {
                            deviceList->at(i)->updateDevice();
                            deviceList->at(i)->updateConfig();
                            deviceList->at(i)->updateState();
                            deviceList->at(i)->deviceDescr.counter_toSend=ui.comboBox_counter_present->isChecked();//ui.lineEdit_counter_pass->text().toInt();
                            deviceList->at(i)->deviceDescr.counter_type=ui.comboBox_counter_type->currentIndex();//сохраняет тип счётчика во временном массиве со сдвигом +1 относительно номеров в терминале
                            deviceList->at(i)->deviceDescr.counter_number=ui.lineEdit_counter_number->text().toInt();
                            deviceList->at(i)->deviceDescr.counter_pass=ui.lineEdit_counter_pass->text().toInt();
                            deviceList->at(i)->deviceDescr.counter_precision=ui.comboBox_counter_precision->currentIndex();
                            i=0xFFFF;
                        }
                    }
                    iShow();
                    emit UpdatedDB();
                //}
            }
            else QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),
                tr("Просьба сообщить об этой ошибке разработчику с описанием действий и доп. информации, вызвавших это сообщение. Дополнительная информация: check validator"), QMessageBox::Cancel,
                QMessageBox::NoButton);//хер его знает что он валидирует, проверить
        }
        else
        {
            QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),
                tr("Терминал с таким именем, серийным номером или телефоном уже существует!"), QMessageBox::Cancel,
                QMessageBox::NoButton);
        }
    }
    else
    {
        QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),
                tr("Поле \"Имя терминала\" должно быть заполнено!"), QMessageBox::Cancel,
                QMessageBox::NoButton);
    }
}

void TerminalsEdit::on_pbExit_clicked()
{
    reIndexPosition();
    emit refreshG();
    hide();
}

void TerminalsEdit::on_pbAddDevice_clicked()
{
    key_prop=true;
    if(key_login)//что это вообще? (!!!убрать!!!)
    {
        //ui.pbDelDevice->setDisabled(true);
        //ui.pbSave->setDisabled(false);
    }
    bool goooood = TRUE;//странный флаг (!!!убрать!!!)
    QSqlQuery *tq = new QSqlQuery();
    tq->exec("select max(position) from device");//добавлять бы его сверху, чтобы в конец не лезть, или потом на него прокручивать список
    tq->next();
    int maxPos = tq->value(0).toInt();

    ////////////////////////////////////////////////////////////

    QString str;
    str="INSERT INTO device (position) VALUES("+QString::number(maxPos+1)+");";//default values from table must be set properly
    //str="insert into device (name,phone,position,counter_type,is_avto,configured,delta_13,delta_14,delta_15,delta_16,data_mode) 
    //values ('"+tr("новый шкаф")+"', '+380674870000', "+QString::number(maxPos+1)+",0,0,0,-1,-1,-1,-1,1)";
    if (!tq->exec(str))
    {
        QSqlError err = tq->lastError();
        QString error_name;
        for (int i=0;i<deviceList->count();i++)
        {
            if (deviceList->at(i)->deviceDescr.phone=="+38067")
            {
                //currRec=deviceList->at(i)->rec_id;
                ui.tableWidget->selectRow(i);//хорошо бы им помигать или подкрасить ещё..
                error_name=deviceList->at(i)->deviceDescr.name;
                break;
            }
        }
        QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),tr("Номер телефона добавленного ранее прибора скорее всего был оставлен по умолчанию. Прибор имеет название \"")+error_name+tr("\".\n\n")+err.text().toUtf8().constData(),QMessageBox::Cancel,QMessageBox::NoButton);
        //old QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),tr("Номер телефона добавленного ранее прибора скорее всего был оставлен по умолчанию. Прибор имеет название \"")+error_name+tr("\".\n\n")+err.text().toAscii().constData(),QMessageBox::Cancel,QMessageBox::NoButton);
        goooood = FALSE;
    }
    else
    {
        //tq->exec("select rec_id from device where phone=""+380674870000""");
        //int newId;
        //if (tq->isActive()){
            //while (tq->next()){
                //newId = tq->value(0).toInt();
            //}
        //}
        int newId=tq->lastInsertId().toInt();
        int phNumber;
        int kNumber;
        for (int i=0;i<9;i++)
        {
            if ((i==0)||(i==3)||(i==6))
            {
                phNumber = 0;
            }
            else
            {
                if((i==1)||(i==4)||(i==7))
                {
                    phNumber = 1;
                }
                else
                {
                    phNumber = 2;
                }
            }
            str.clear();
            str="insert into shuno_config "
                "(device_id, ckeck_point_number, k_number, phase_number, human_name, type, is_present) "
                "values ("+QString::number(newId)+", "+QString::number(i+1)+", "+QString::number((i<3)?(0):(1))
                +", "+QString::number(phNumber)+", '"+tr("КТ")+"', 0, 1)";
            if (!tq->exec(str))
            {
                QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),tr("Ошибка"), QMessageBox::Cancel,QMessageBox::NoButton);
                goooood = FALSE;
            }
        }
        ///////////////////////////////////////////
        str.clear();
        str="insert into dev_state (device_id) values("+QString::number(newId)+")";
        if (!tq->exec(str))
        {
            QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),tr("Ошибка"), QMessageBox::Cancel, QMessageBox::NoButton);
            goooood = FALSE;
        }
        str="insert into recv_state (device_id,raw_data,time_stamp,is_error,human_descr,vm_id,port_id) values("+QString::number(newId)+",57344,NOW(),0,'"+tr("Новый терминал добавлен.")+"',0,0)";
        if (!tq->exec(str))
        {
            QMessageBox::critical(0, qApp->tr("Ошибка создания записи"),tr("Ошибка"), QMessageBox::Cancel, QMessageBox::NoButton);
            goooood = FALSE;
        }
        if (goooood)
        {
            QTableWidgetItem *tItem;
            //////////////////////////////////////////////////////////////////////////??????????????????????????????
            IPObject *tO = new IPObject(parent,newId);
            deviceList->append(tO);
            currRec = newId;
            iShow();
            emit UpdatedDB();
        }
    }
    delete tq;
}

void TerminalsEdit::on_pbDelDevice_clicked()
{
    if(key_login) ui.pbDelDevice->setDisabled(true);
    if (refrEnable){//мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?) (!!!убрать!!!)
        QSqlQuery *tq = new QSqlQuery();
        //tq->prepare("update device set is_active=0 where rec_id="+QString::number(currRec,10));
        tq->prepare("delete from device where rec_id="+QString::number(currRec,10));
        if (!tq->exec()){
            QMessageBox::critical(0, qApp->tr("Ошибка удаления записи"),
                tr("Ошибка"), QMessageBox::Cancel,
                QMessageBox::NoButton);
        }else{
            //прибить его в списке
            int devListLen = deviceList->size();
            int currDevListIndex=-1;
            for (int i=0;i<devListLen;i++){
                if (deviceList->at(i)->rec_id == currRec){
                    delete deviceList->takeAt(i);
                    /*QString s;
                    s.append(QString::number(devListLen,10));
                    QMessageBox::critical(0, qApp->tr("Ошибка"),s, QMessageBox::Ok,QMessageBox::NoButton);*/
                    if(devListLen==1)
                    {
                        on_pbSave_clicked();
                        ui.pbSave->setDisabled(true);
                    }
                    else
                        ui.tableWidget->removeRow(i);
                    i--;
                    if (i>=0){
                        currRec = deviceList->at(i)->rec_id;
                        ui.tableWidget->setCurrentCell(i,0);
                        BDrefresh(currRec);
                    }
                    else currRec=-1;
                    i=0xFFFF;
                }
            }
            //удалить из таблицы
            ///////////////////////////////////////////////////////
                ////////ЁПРСТ
            int buf[4];
                QSqlQuery *tq = new QSqlQuery();
                tq->prepare("update device set "
                    "name = :name, "
                    "phone = :phone, "
                    "adress = :adress, "
                    "dev_hard_number = :dev_hard_number, "
                    "group_g = :group_g, "
                    "group_a = :group_a, "
                    //"avtonom_present = :avtonom_present, "
                    //"counter_present = :counter_present, "
                    //"counter_precision = :counter_precision, "
                    //"counter_number = :counter_number, "
                    //"counter_type = :counter_type, "
                    //"door_present = :door_present, "
                    "notes = :notes, "
                    "x = :x, "
                    "y = :y, "
                    "photo = :photo, "
                    "is_root = :is_root "
                    "where rec_id = "+QString::number(currRec,10)
                    );
                tq->bindValue(":name",ui.le_name->text());
                tq->bindValue(":phone",ui.le_phone->text());
                tq->bindValue(":adress",ui.le_adres->text());
                tq->bindValue(":dev_hard_number",ui.le_dev_hard->text().toInt());
                tq->bindValue(":group_g",ui.combo_group_g->itemData(ui.combo_group_g->currentIndex(),Qt::UserRole).toInt());
                tq->bindValue(":group_a",ui.combo_group_a->itemData(ui.combo_group_a->currentIndex(),Qt::UserRole).toInt());
                
                /*if (ui.cb_avtonom_present->checkState() == Qt::Checked)
                {
                    tq->bindValue(":avtonom_present",TRUE); 
                    buf[0]=1;
                }
                else
                {
                    tq->bindValue(":avtonom_present",FALSE);
                    buf[0]=0;
                }*/
                if (ui.cbRoot->checkState() == Qt::Checked)
                {
                    tq->bindValue(":is_root",TRUE); 
                    buf[1]=1;
                }
                else
                {
                    tq->bindValue(":is_root",FALSE);    
                    buf[1]=0;
                }
                /*if (ui.comboBox_counter_present->checkState() == Qt::Checked)
                {
                    tq->bindValue(":counter_present",TRUE);
                    buf[2]=1;
                }
                else
                {
                    tq->bindValue(":counter_present",FALSE);
                    buf[2]=0;
                }*/
                
                /*tq->bindValue(":counter_precision",ui.comboBox_counter_precision->currentIndex());
                tq->bindValue(":counter_number",ui.lineEdit_counter_number->text().toInt());
                tq->bindValue(":counter_type",ui.comboBox_counter_type->currentIndex());*/

                
                /*if (ui.cb_door_present->checkState() == Qt::Checked)
                {
                    tq->bindValue(":door_present",TRUE);
                    buf[3]=1;
                }
                else
                {
                    tq->bindValue(":door_present",FALSE);
                    buf[3]=0;
                }*/
                tq->bindValue(":notes",ui.te_notes->toPlainText());
                tq->bindValue(":x",ui.le_x->text().toFloat());
                tq->bindValue(":y",ui.le_y->text().toFloat());
                //photottotototo.....
                //tq->bindValue(":photo",ui.photo_label->pixmap());
                //if (ui.photo_label->pixmap()){
                //  QPixmap pM(*ui.photo_label->pixmap());
                //  tq->bindValue(":photo",QVariant(pM));
                    //tq->bindValue(":photo",0);
                //  ui.photo_label->setPixmap(pM);
                //} else{
                    tq->bindValue(":photo",0);
                //}
                //QString debugStr = tq->lastQuery();

               /* QString s,s1,s2;
                s1=s.setNum(ui.le_x->text().toFloat());
                s.clear();
                s2=s.setNum(ui.le_y->text().toFloat());
                QString str="update device set name ='"+ui.le_name->text()+"', phone ='"+ui.le_phone->text()
                    +"', adress ='"+ ui.le_adres->text()+"', dev_hard_number ="+QString::number(ui.le_dev_hard->text().toInt(),10)
                    +", group_g ="+QString::number(ui.combo_group_g->itemData(ui.combo_group_g->currentIndex(),Qt::UserRole).toInt(),10)
                    +", group_a ="+QString::number(ui.combo_group_a->itemData(ui.combo_group_a->currentIndex(),Qt::UserRole).toInt(),10)
                    +", avtonom_present ="+QString::number(buf[0],10)+", counter_present ="+QString::number(buf[2],10)+", counter_precision ="
                    +QString::number(ui.combo_counter_precision->currentIndex(),10)+", door_present ="+QString::number(buf[3],10)+", notes ='"+ui.te_notes->toPlainText()+"', x ="
                    +s1+", y="+s2+", photo =0, is_root ="+QString::number(buf[1],10)+" where rec_id = "+QString::number(currRec,10);*/
                if (!tq->exec())
                {
                    QSqlError err = tq->lastError();
                    QMessageBox::critical(0, qApp->tr("Ошибка сохранения"),
                        err.text().toUtf8().constData()/*tr("Неверные введенные данные")*/, QMessageBox::Cancel,
                        // old err.text().toAscii().constData()/*tr("Неверные введенные данные")*/, QMessageBox::Cancel,
                        QMessageBox::NoButton);
                }
                //////////////////////////////////////////////////////////////////////////
                devListLen = deviceList->size();
                currDevListIndex=-1;
                for (int i=0;i<devListLen;i++)
                {
                    if (deviceList->at(i)->rec_id == currRec)
                    {
                        deviceList->at(i)->updateDevice();
                        deviceList->at(i)->updateConfig();
                        deviceList->at(i)->updateState();
                        i=0xFFFF;
                    }
                }
                iShow();
                emit UpdatedDB();
        }
    }
    
}

void TerminalsEdit::on_tableWidget_currentItemChanged(QTableWidgetItem* current,QTableWidgetItem* prev)
{
    if (refrEnable){//мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?) (!!!убрать!!!)
        key_prop=false;
        int row = current->row();
        currRow = row;
        int bdIndex = ui.tableWidget->item(row,0)->text().toInt(NULL,10);
        currRec = bdIndex;
        BDrefresh(currRec);
        //terminal_cursor_position=ui.tableWidget->model()->index(current->row(),1);
        //terminal_cursor_position_int=current->row();
        if (ui.tableWidget->model()->index(row,1).isValid())
        {
            if(key_login) ui.pbDelDevice->setDisabled(false);
        }
    }
}

void TerminalsEdit::BDrefresh(int baseIndex){
        //search
    int devListLen = deviceList->size();
    int currDevListIndex=-1;
    for (int i=0;i<devListLen;i++){
        if (deviceList->at(i)->rec_id == baseIndex){
            currDevListIndex = i;
            i=0xFFFF;
        }
    }
    if (currDevListIndex != -1)
    {

        QSqlQuery *tq = new QSqlQuery();
        
        ui.cb_datamode->setCheckState(Qt::Unchecked);
        //ui.cb_voicemode->setCheckState(Qt::Unchecked);
        if(tq->exec("select data_mode from device where rec_id="+QString::number(currRec,10)))
        {
            if (tq->isActive())
            {
                while (tq->next())
                {
                    bool terminal_mode=tq->value(0).toBool();
                    //if(CHECK_BIT(terminal_mode,0))	ui.cb_voicemode->setCheckState(Qt::Checked);
                    //if(CHECK_BIT(terminal_mode,1))	ui.cb_datamode->setCheckState(Qt::Checked);
                    if (terminal_mode) ui.cb_datamode->setCheckState(Qt::Checked);
                }
            }
        }

        ui.le_name->setText(deviceList->at(currDevListIndex)->deviceDescr.name);
        ui.le_phone->setText(deviceList->at(currDevListIndex)->deviceDescr.phone);
        ui.le_dev_hard->setText(QString::number(deviceList->at(currDevListIndex)->deviceDescr.dev_hard_number,10));
        ui.le_adres->setText(deviceList->at(currDevListIndex)->deviceDescr.adress);
        ui.te_notes->setPlainText(deviceList->at(currDevListIndex)->deviceDescr.notes);
        ui.le_x->setText(QString::number(deviceList->at(currDevListIndex)->deviceDescr.x,'f',3));
        ui.le_y->setText(QString::number(deviceList->at(currDevListIndex)->deviceDescr.y,'f',3));
        //groups
        int gIndex = ui.combo_group_g->findData(deviceList->at(currDevListIndex)->deviceDescr.group_g);
        if (gIndex == -1) gIndex = 0;
        ui.combo_group_g->setCurrentIndex(gIndex);
        gIndex = ui.combo_group_a->findData(deviceList->at(currDevListIndex)->deviceDescr.group_a);
        if (gIndex == -1) gIndex = 0;
        ui.combo_group_a->setCurrentIndex(gIndex);
        //options
        /*if (!deviceList->at(currDevListIndex)->deviceDescr.door_present){
            ui.cb_door_present->setCheckState(Qt::Unchecked);
        }else{
            ui.cb_door_present->setCheckState(Qt::Checked);
        }*/
        /*if (!deviceList->at(currDevListIndex)->deviceDescr.avtonom_present){
            ui.cb_avtonom_present->setCheckState(Qt::Unchecked);
        }else{
            ui.cb_avtonom_present->setCheckState(Qt::Checked);
        }*/
        if (deviceList->at(currDevListIndex)->deviceDescr.counter_toSend)
        {
            ui.comboBox_counter_present->setCheckState(Qt::Checked);
            ui.comboBox_counter_precision->setDisabled(FALSE);
            ui.lineEdit_counter_number->setDisabled(FALSE);
            ui.lineEdit_counter_pass->setDisabled(FALSE);
            ui.comboBox_counter_type->setDisabled(FALSE);
        }
        else
        {
            ui.comboBox_counter_present->setCheckState(Qt::Unchecked);
            ui.comboBox_counter_precision->setDisabled(TRUE);
            ui.lineEdit_counter_number->setDisabled(TRUE);
            ui.lineEdit_counter_pass->setDisabled(TRUE);
            ui.comboBox_counter_type->setDisabled(TRUE);
        }
        if ((deviceList->at(currDevListIndex)->deviceDescr.counter_precision >= 0)&&(deviceList->at(currDevListIndex)->deviceDescr.counter_precision < 5))
        {   ui.comboBox_counter_precision->setCurrentIndex(deviceList->at(currDevListIndex)->deviceDescr.counter_precision);    }
        else
        {   ui.comboBox_counter_precision->setCurrentIndex(0);  }
        //if ((deviceList->at(currDevListIndex)->deviceDescr.counter_number !=0))
        //{ 
        ui.lineEdit_counter_number->setText(QString::number(deviceList->at(currDevListIndex)->deviceDescr.counter_number,10));  
        //}
        ///else
        //{ ui.lineEdit_counter_number->setText("-1");  }
        if ((deviceList->at(currDevListIndex)->deviceDescr.counter_pass !=-1))
        {   ui.lineEdit_counter_pass->setText(QString::number(deviceList->at(currDevListIndex)->deviceDescr.counter_pass,10));  }
        else
        {   ui.lineEdit_counter_pass->setText("-1");    }
        //if ((deviceList->at(currDevListIndex)->deviceDescr.counter_type > 0)&&(deviceList->at(currDevListIndex)->deviceDescr.counter_type < 9))
        //{
        ui.comboBox_counter_type->setCurrentIndex(deviceList->at(currDevListIndex)->deviceDescr.counter_type);
        //}
        //else
        //{ ui.comboBox_counter_type->setCurrentIndex(0);   }
        //ROOT
        if(deviceList->at(currDevListIndex)->deviceDescr.is_root)
        {   ui.cbRoot->setCheckState(Qt::Checked);  }
        else
        {   ui.cbRoot->setCheckState(Qt::Unchecked);    }

        delete tq;
    }
}

void TerminalsEdit::on_pushButton_clicked()
{
    //load pic
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName != ""){
        QPixmap tP;
        if (tP.load(fileName)){
            tP = tP.scaled(QSize(128,170),Qt::KeepAspectRatio,Qt::FastTransformation);
            ui.photo_label->setPixmap(tP);
        }
    }
}

void TerminalsEdit::on_pushButton_2_clicked()
{
    ui.photo_label->clear();
}

void TerminalsEdit::on_pbUpJump_clicked()
{
    //+10
    if (currRow >= 0){
        int upRow = currRow-10;
        if (upRow<0)upRow=0;
        if (upRow != currRow){
            deviceList->swap(currRow,upRow);
            iShow();
            
        }
        ui.tableWidget->setCurrentCell(upRow,1);
        currRow = upRow;
    }
}

void TerminalsEdit::on_pbUpInc_clicked()
{
    //+1
    if (currRow >= 0){
        int upRow = currRow-1;
        if (upRow<0)upRow=0;
        if (upRow != currRow){
            deviceList->swap(currRow,upRow);
            iShow();
            
        }
        ui.tableWidget->setCurrentCell(upRow,1);
        currRow = upRow;
    }
}

void TerminalsEdit::on_pbDownInc_clicked()
{
    //-1
    if (currRow >= 0){
        int downRow = currRow+1;
        if (downRow>deviceList->size()-1) downRow=deviceList->size()-1;
        if (downRow != currRow){
            deviceList->swap(currRow,downRow);
            iShow();
            
        }
        ui.tableWidget->setCurrentCell(downRow,1);
        currRow = downRow;
    }
}

void TerminalsEdit::on_pbDownJump_clicked()
{
    //-10
    if (currRow >= 0){
        int downRow = currRow+10;
        if (downRow>deviceList->size()-1) downRow=deviceList->size()-1;
        if (downRow != currRow){
            deviceList->swap(currRow,downRow);
            iShow();
            
        }
        ui.tableWidget->setCurrentCell(downRow,1);
        currRow = downRow;
    }
}

void TerminalsEdit::reIndexPosition(){
    int devListLen = deviceList->size();
    QSqlQuery tq;
    for (int i=0;i<devListLen;i++){
        deviceList->at(i)->deviceDescr.position = i;
        tq.exec("update device set position = "+QString::number(i,10)+" where rec_id = " 
            + QString::number(deviceList->at(i)->rec_id));
    }
}

void TerminalsEdit::setAccessLevel(){
    QList<QObject *> childrenList = findChildren<QObject *>();
    QSqlQuery tq;
    QWidget *tW;
    QAction *tA;
    BaseSettings *mainBs = BaseSettings::createInstance();
    for (int i=0;i<childrenList.count();i++){
        if (childrenList.at(i)->objectName() != ""){
            //"monitor_Monitor_"+childrenList.at(i)->objectName() 
            QString str="select is_enabled from roles_access where name = 'monitor_TerminalsEdit_"+
                childrenList.at(i)->objectName()+"' and role = "+QString::number(mainBs->accessLevel,10);
            if (!tq.exec("select is_enabled from roles_access where name = 'monitor_TerminalsEdit_"+
                childrenList.at(i)->objectName()+
                "' and role = "+QString::number(mainBs->accessLevel,10))){
                    //error
                    QSqlError err = tq.lastError();
                    QMessageBox::critical(0, tr("Ошибка выполнения запроса"),
                        err.text(), QMessageBox::Cancel,
                        QMessageBox::NoButton);
            }else{
                //ok
                if (tq.next()){
                    tW = dynamic_cast<QWidget*>(childrenList.at(i));
                    if (tW){
                        tW->setDisabled(!tq.value(tq.record().indexOf("is_enabled")).toBool());
                        key_login=tq.value(tq.record().indexOf("is_enabled")).toBool();
                    }else{
                        tA = dynamic_cast<QAction*>(childrenList.at(i));
                        if (tA){
                            tA->setDisabled(!tq.value(tq.record().indexOf("is_enabled")).toBool());
                        }
                    }
                }
            }
        }
    }
    mainBs->freeInst();
}


void TerminalsEdit::dbclicked(QModelIndex model)
{
    fnd_terminal->setParent(this);
    fnd_terminal->iShow();
}

void TerminalsEdit::nameTextChanged(const QString & string)
{
    QRegExp rx("\\d+");
    int pos = rx.indexIn(string);
    int name_numm = rx.capturedTexts().first().toInt();
    QPalette palette (ui.le_name->palette());
    if (name_numm)
    {
    palette.setColor(QPalette::Text,Qt::darkGreen);
    ui.le_name->setPalette(palette);
    }
    else
    {
    palette.setColor(QPalette::Text,Qt::darkRed);
    ui.le_name->setPalette(palette);
    }
}

void TerminalsEdit::setPositionView()
{
    ui.tableWidget->setCurrentCell(position_view,position_key);
}

void TerminalsEdit::on_pb_print_clicked()
{
    ptr=NULL;
    ptr=new PR_EXCEL(NULL,"",this->geometry(),tr("Выполняется EXCEL скрипт..."));
    ptr->setParent(this);
    ptr->key_view=true;
    ptr->PrintData();
    if(ptr) delete ptr;
}

void TerminalsEdit::setParent(monitor* prt)
{
    parent=(monitor*)prt;
}
