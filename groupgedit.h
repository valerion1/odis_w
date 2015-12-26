#ifndef GROUPGEDIT_H
#define GROUPGEDIT_H

#include <QtWidgets/QWidget>
#include "ui_groupgedit.h"
#include "monitor.h"
#include <QtWidgets/QHeaderView>

class QSqlTableModel;
class monitor;

class GroupGEdit : public QWidget
{
    Q_OBJECT

public:
    GroupGEdit(QWidget *parent = 0);
    ~GroupGEdit();

protected:
	void closeEvent(QCloseEvent *event);

private:
    monitor* parent;
    Ui::GroupGEditClass ui;
    QSqlDatabase tDB;
	QSqlTableModel *model;
    QString selname;
    void keyPressEvent(QKeyEvent *event);
    void saveColorInfo();
    void initializeModel(QSqlTableModel *model);

public slots:
    void iShow();
    void setParent(monitor*);
    void sortChanged(int);
//  void on_t b_clicked(QModelIndex);

private slots:
    void on_pbExit_clicked();
    void on_pbDel_clicked();
    void on_pbAdd_clicked();
    void refresh();
    
signals:
    void refreshG();
    void clicked(QModelIndex);
    void sectionClicked(int);
};

#endif // GROUPGEDIT_H
