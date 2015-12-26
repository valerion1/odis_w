#ifndef GROUPAEDIT_H
#define GROUPAEDIT_H

#include <QtWidgets/QWidget>
#include "ui_groupaedit.h"
#include "monitor.h"

class QSqlTableModel;
class monitor;

class GroupAEdit : public QWidget
{
    Q_OBJECT

public:
    GroupAEdit(QWidget *parent = 0);
    ~GroupAEdit();
    void setParent(monitor*);

protected:
	void closeEvent(QCloseEvent *event);

private:
    Ui::GroupAEditClass ui;
    monitor* parent;
    QSqlDatabase tDB;
    QSqlTableModel *model;
    void keyPressEvent(QKeyEvent *event);
    void saveColorInfo();
    void initializeModel(QSqlTableModel *model);

public slots:
    void iShow();

private slots:
    void on_pbExit_clicked();
    //void on_pbProgramm_clicked();
    void on_pbDel_clicked();
    void on_pbAdd_clicked();
    void refresh(QString old_string);
    //void refresh(const QModelIndex &topLeft);
    void endEdit(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    //void on_t b_clicked(QModelIndex);//этого нет в .ui
    void sortChanged(int);

signals:
    void refreshG();
    void clicked(QModelIndex);
    void sectionClicked(int);
};

#endif // GROUPAEDIT_H
