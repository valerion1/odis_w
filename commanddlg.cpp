#include "commanddlg.h"
//#include "senddatadlg.h"

#include <QCloseEvent>
#include <QKeyEvent>
#include <QMessageBox>

CommandDialog::CommandDialog(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(this, SIGNAL(clicked()),this, SLOT(close()));
    connect(ui.cb_kontaktors, SIGNAL(clicked()),this, SLOT(on_kontaktors()));
    connect(ui.cb_servise, SIGNAL(clicked()),this, SLOT(on_servise()));
//  connect(ui.cb_anycommand, SIGNAL(clicked()),this, SLOT(on_anycommands()));
    


    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::Window|/*Qt::WindowSystemMenuHint|*/Qt::WindowTitleHint);
    this->setWindowTitle(tr("Управление прибором"));
    this->setFixedSize(270,430);

            
}

CommandDialog::~CommandDialog()
{

}

void CommandDialog::iShow()
{
    ui.cb_kontaktors->setChecked(false);

    ui.rb_kn_on_kv_off->setChecked(true);
    ui.rb_kn_on_kv_off->setDisabled(true);

    ui.rb_rn_off_kv_on->setChecked(false);
    ui.rb_rn_off_kv_on->setDisabled(true);

    ui.rb_group_off->setChecked(false);
    ui.rb_group_off->setDisabled(true);

    ui.rb_group_on->setChecked(false);
    ui.rb_group_on->setDisabled(true);

    ui.rb_par->setChecked(false);
    ui.rb_par->setDisabled(true);

    
////////////////////////////////////////////////////////
    ui.cb_servise->setChecked(false);

    ui.rb_fullreset->setChecked(true);
    ui.rb_fullreset->setDisabled(true);

    ui.rb_loadpar->setChecked(false);
    ui.rb_loadpar->setDisabled(true);

    ui.rb_loadprog->setChecked(false);
    ui.rb_loadprog->setDisabled(true);

    ui.rb_progreset->setChecked(false);
    ui.rb_progreset->setDisabled(true);
    
////////////////////////////////////////////////////////
//  ui.cb_anycommand->setChecked(false);
    ui.cb_door->setChecked(false);
//  ui.le_anytext->setDisabled(true);

    show();
}


void CommandDialog::on_pushButton_clicked()
{
    QString str;

    parent->COMM_STR.clear();
    str.clear();

    if(ui.cb_servise->isChecked())
    {
        if(ui.rb_fullreset->isChecked())
            str.append("UC=15;");
        else if(ui.rb_progreset->isChecked())
            str.append("UC=1;");
        else if(ui.rb_loadprog->isChecked())
            str.append("UC=19;");
        else if(ui.rb_loadpar->isChecked())
            str.append("UC=20;");
    }
    else
    {
        if(ui.cb_kontaktors->isChecked())
        {
            if(ui.rb_kn_on_kv_off->isChecked())
                str.append("UC=9;");
            else if(ui.rb_rn_off_kv_on->isChecked())
                str.append("UC=10;");
            else if(ui.rb_group_on->isChecked())
                str.append("UC=11;");
            else if(ui.rb_group_off->isChecked())
                str.append("UC=8;");
            else if(ui.rb_par->isChecked())
                str.append("UC=12;");
        }

        if(ui.cb_door->isChecked())
            str.append("UC=4;");
        /*if(ui.cb_anycommand->isChecked())
        {
            if(!ui.le_anytext->text().isEmpty())
                str.append("UC=23,"+ui.le_anytext->text()+";");
            else
            {
                QMessageBox::critical(0,tr("Управление прибором"),tr("Не задана произвольная команда!"),QMessageBox::Ok,QMessageBox::NoButton);
                return;
            }
        }*/
    }
    if(!str.isEmpty())
    {
        parent->COMM_STR.append(str);
        emit ToServer(3);
    }

    hide();
}

void CommandDialog::on_pushButton_2_clicked()
{
    hide();
}
/*void CommandDialog::on_anycommands()
{
    ui.le_anytext->setText("");
    if(ui.cb_anycommand->isChecked())
    {
        ui.le_anytext->setDisabled(false);
    }
    else
    {
        ui.le_anytext->setDisabled(true);
    }
}*/
void CommandDialog::on_kontaktors()
{
    if(ui.cb_kontaktors->isChecked())
    {
        ui.rb_kn_on_kv_off->setChecked(true);
        ui.rb_kn_on_kv_off->setDisabled(false);

        ui.rb_rn_off_kv_on->setChecked(false);
        ui.rb_rn_off_kv_on->setDisabled(false);

        ui.rb_group_off->setChecked(false);
        ui.rb_group_off->setDisabled(false);

        ui.rb_group_on->setChecked(false);
        ui.rb_group_on->setDisabled(false);

        ui.rb_par->setChecked(false);
        ui.rb_par->setDisabled(false);
    }
    else
    {
        ui.rb_kn_on_kv_off->setChecked(true);
        ui.rb_kn_on_kv_off->setDisabled(true);

        ui.rb_rn_off_kv_on->setChecked(false);
        ui.rb_rn_off_kv_on->setDisabled(true);

        ui.rb_group_off->setChecked(false);
        ui.rb_group_off->setDisabled(true);

        ui.rb_group_on->setChecked(false);
        ui.rb_group_on->setDisabled(true);

        ui.rb_par->setChecked(false);
        ui.rb_par->setDisabled(true);
    }

    
}
void CommandDialog::on_servise()
{
    if(ui.cb_servise->isChecked())
    {
        ui.cb_kontaktors->setChecked(false);
        ui.cb_kontaktors->setDisabled(true);

        ui.rb_kn_on_kv_off->setChecked(true);
        ui.rb_kn_on_kv_off->setDisabled(true);

        ui.rb_rn_off_kv_on->setChecked(false);
        ui.rb_rn_off_kv_on->setDisabled(true);

        ui.rb_group_off->setChecked(false);
        ui.rb_group_off->setDisabled(true);

        ui.rb_group_on->setChecked(false);
        ui.rb_group_on->setDisabled(true);

        ui.rb_par->setChecked(false);
        ui.rb_par->setDisabled(true);

        
        ui.cb_door->setChecked(false);
        ui.cb_door->setDisabled(true);

        

//      ui.cb_anycommand->setChecked(false);
//      ui.cb_anycommand->setDisabled(true);
//      ui.le_anytext->setDisabled(true);
//      ui.le_anytext->setText("");


        ui.rb_fullreset->setChecked(true);
        ui.rb_fullreset->setDisabled(false);
        ui.rb_loadpar->setDisabled(false);
        ui.rb_loadprog->setDisabled(false);
        ui.rb_progreset->setDisabled(false);

    }
    else
    {
        ui.cb_kontaktors->setChecked(false);
        ui.cb_kontaktors->setDisabled(false);
        
        ui.rb_fullreset->setChecked(true);
        ui.rb_fullreset->setDisabled(true);

        ui.rb_loadpar->setChecked(false);
        ui.rb_loadpar->setDisabled(true);

        ui.rb_loadprog->setChecked(false);
        ui.rb_loadprog->setDisabled(true);

        ui.rb_progreset->setChecked(false);
        ui.rb_progreset->setDisabled(true);

            
        ui.cb_door->setChecked(false);
        ui.cb_door->setDisabled(false);

        

//      ui.cb_anycommand->setChecked(false);
//      ui.cb_anycommand->setDisabled(false);
//      ui.le_anytext->setDisabled(true);
//      ui.le_anytext->setText("");

        on_kontaktors();


    }
}
void CommandDialog::setParent(IPWindow* prt)
{
    parent=(IPWindow*)prt;
    
}
void CommandDialog::closeEvent(QCloseEvent *event)
{
    event->accept();   
   
}
void CommandDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        hide();
    }
}



