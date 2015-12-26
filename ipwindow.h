#ifndef IPWINDOW_H
#define IPWINDOW_H

#include <QtWidgets/QWidget>
// old #include <QWidget>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QtWidgets/QPushButton>
// old #include <QPushButton>
#include <QtGui>

#include "ui_ipwindow.h"
#include "notesipw.h"
#include "lavelhistory.h"
//#include "senddatadlg.h"
//#include "getdatadlg.h"
//#include "commanddlg.h"
#include "more_counter.h"
#include "datatypes.h"

#define CHECK_BIT(var,pos) (((var) & (1<<(pos)))>0)
#define SET_BIT(p,n) ((p) |= (1 << (n)))
#define CLR_BIT(p,n) ((p) &= (~(1) << (n)))

class lavelhistory;
class QSqlQueryModel;
class IPObject;
//class IPConfigurator;
class BaseSettings;
class LogModel;
class notesIPW;
class monitor;
//class GetDataDialog;
//class SendDataDialog;
//class CommandDialog;
class more_counter;


/*class ItemDelegate : public QItemDelegate
{
public:
  ItemDelegate()
  {}
  QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
  {
    return QSize(20,20);
  }
};*/


class IPWindow : public QWidget
{
    Q_OBJECT

public:
    
    IPWindow(QWidget *parent = 0);
    ~IPWindow();
    
    struct full_delta//описание одного шага
    {
        bool primary;//0=дополнительные дельты, 1=основные
        int value;//значение дельты, для основных шагов
        QTime time;//время для дополнительных шагов
        quint8 event;//КН КВ для дополнительных шагов
    };
    full_delta delta[16];//обьявление массива из шагов
    _step stepsIPW[8];//массив шагов для сортировки и вывода в отображении шагов + дельт
    void refreshParSteps(_step *psteps);

    monitor *parent;
    IPObject *master;
    lavelhistory *history;
    bool key_login;
    bool key_solut;
    bool key_count;
    bool key_anycommand;
    bool key_config;
    int dbIndex;
    bool key_anim;
    int count_anim;
    bool key_hisgsm;
    //QTimer* tm_state;
    //QTimer* tm_load;

    QPushButton *pbMskRequest,*pbDeltaRequest,*pbSetData;
    
    void setParent(monitor*);
    void SignaltoMap(IPObject *in_master);
    //void setMyConfig();
    bool checkTime();
    void DeltaEdit(IPObject*,QDate,int);
    void drawSHUNO(int dbIndex);
    void sortParSteps(_step *psteps);
    IPObject* getObject();

    //GetDataDialog* getdlg;
    //SendDataDialog* setdlg;
    //CommandDialog* senddlg;
    //void SendData(QString,int);
    int delta_time;
    int buttons_config; 

public slots:
    void iShow(IPObject *in_master);
    //void ipconf_putCommand(int,int,int);
    //void ipconf_signalAllToServer();
    void refreshFace();
    void setAccessLevel();
    void on_pbSendManualCmd_clicked();
    //void tm_stateTimer();
    //void tm_loadTimer();

private:
    Ui::IPWindowClass ui;
    bool split_par;
    QImage image,face,face1;
    notesIPW* notedlg;
    QString n_name,notes;
    QTimer* anim;
    more_counter* moredlg;
    LogModel *model;
    int k1;
    int k2;
    //int num_state;
    //int state_inter;
    int k1_old,k2_old;
    //QTimeEdit *step_time[8];//время шага
    QLabel      *step_time[8];//время шага
    QLabel      *step_delta[8];//пиктограмма дельты/дополнительного шага
    QLabel      *step_event[8];//картинка с лампочками для индикации события шага
    QLabel      *step_text[8];//текстовое описание шага, непонятно, правда, зачем, но зачем-то раньше выводилось

    //void paintEvent(QPaintEvent * event);
    void drawHistory();
    void keyPressEvent (QKeyEvent *event);
    //void eventFilter(QObject *obj,QEvent *event);
    void RedrawAutonomSteps(void);
    static int sort_steps(const void * p1,const void * p2); //функция сортировки двумерного массива по времени с сохранением номеров шагов
    
signals:
    void putCommand(int device, QString command, int command_type);
    //void putCommand(int device, int command, int command_type);
    void putMap(int device);
    void refreshList();
    void signalToServer();
    //void signalAllToServer();
    
private slots:
    //void on_pbSendConfig_clicked();
    //void on_pushButton_6_clicked();
    //void on_pushButton_10_clicked();
    //void on_pushButton_11_clicked();
    //void on_pushButton_13_clicked();
    //void on_pushButton_14_clicked();
    //void on_pushButton_17_clicked();

    void on_pbK2_clicked();
    void on_pbK1_clicked();
    void on_pb_requesttime_clicked();
    void on_pb_par_clicked();
    void on_pb_state_clicked();
    void on_pb_sendtime_clicked();
    void on_pb_counter_clicked();
    void pushButton_DeltaRequest_clicked();
    void pushButton_MskRequest_clicked();

    void on_pbMoreButtons_clicked();
    void on_pushButton_ipConf_clicked();
    void on_pbDelQueryCommands_clicked();
    void on_pb_notes_clicked();
    void on_pb_close_clicked();

    void onanimTimer();
    void adrr_click();
    bool eventFilter(QObject *obj, QEvent *ev);
    void closeEvent(QCloseEvent *event);
    //void ToServer(int);
};

#endif // IPWINDOW_H
