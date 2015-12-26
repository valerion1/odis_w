#ifndef COLORSELECT_H
#define COLORSELECT_H

#include <QtWidgets/QWidget>
#include "ui_colorselect.h"

class monitor;

class ColorSelect : public QWidget
{
    Q_OBJECT

public:
    ColorSelect(QWidget *parent = 0);
    ~ColorSelect();
    monitor* parent;
    void setpParent(monitor* prt);

private:
    Ui::ColorSelectClass ui;

public slots:
    void iShow();

signals:
    void refreshG();
    void clicked(QModelIndex);

private slots:
    void on_pbChange_clicked();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem*);
    void on_pbSave_clicked();
    void on_pbClose_clicked();
    void saveColorInfo();
	void slot_SelectionChanged(const QItemSelection &, const QItemSelection &);
    //void on_t b_clicked(QModelIndex);
};

#endif // COLORSELECT_H
