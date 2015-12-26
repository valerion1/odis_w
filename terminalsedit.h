#ifndef TERMINALSEDIT_H
#define TERMINALSEDIT_H

#include <QtWidgets/QWidget>
#include "ui_terminalsedit.h"
#include "datatypes.h"
#include "find_terminal.h"
#include "pr_excel.h"
#include "ipwindow.h"

class BaseSettings;
class IPConfigurator;
class Find_terminal;
class PR_EXCEL;

class TerminalsEdit : public QWidget
{
    Q_OBJECT

public:
    Ui::TerminalsEdit ui;
	QModelIndex terminal_cursor_position;
	int terminal_cursor_position_int;

    bool key_login;
    TerminalsEdit(QWidget *parent = 0,DevList *in_deviceList=0);
    ~TerminalsEdit();
    DevList *deviceList;
    int position_view;
    int position_key;
    void setPositionView();
    void setParent(monitor*);

public slots:
    void iShow();
    void setAccessLevel();
    
private:
    void keyPressEvent(QKeyEvent *event);
    BaseSettings *mainBs;
    PR_EXCEL *ptr;
    monitor* parent;
    int currRec;
    int currRow;
    bool refrEnable;//мутный ключ, проверяется в создании прибора, удалении и обновлении списка(?) (!!!убрать!!!)
    void BDrefresh(int baseIndex);
    bool validator();
    IPConfigurator *ipConf;
    Find_terminal* fnd_terminal;
    bool key_prop;//ещё один мутный ключ, пока не ясно что делает (!!!убрать!!!)
    QPushButton *pb_print;
    QLayout *horizontalLayout;
    
    
private slots:
    void on_pb_print_clicked();
    void on_pbDownJump_clicked();
    void on_pbDownInc_clicked();
    void on_pbUpInc_clicked();
    void on_pbUpJump_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_tableWidget_currentItemChanged(QTableWidgetItem*,QTableWidgetItem*);
    
    void reIndexPosition();

    void on_pbDelDevice_clicked();
    void on_pbAddDevice_clicked();
    void on_pbExit_clicked();
    void on_pbSave_clicked();
    void on_comboBox_counter_present_stateChanged(int newmode);
    //void on_t b_clicked(QModelIndex);//этого нет в .ui
    void dbclicked(QModelIndex);
    void nameTextChanged(const QString &);
signals:
    void refreshG();
    void clicked(QModelIndex);
    void doubleClicked(QModelIndex);
    void UpdatedDB();
    
};

#endif // TERMINALSEDIT_H
