#include "options.h"
#include "basesettings.h"

#include <QCloseEvent>
#include <QKeyEvent>

options::options(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    //connect(this, SIGNAL(clicked()),this, SLOT(close()));
    connect(ui.pb_close, SIGNAL(clicked()),this, SLOT(close()));
    connect(ui.rb_asc, SIGNAL(clicked()),this, SLOT(onSortAsc()));
    connect(ui.rb_desc, SIGNAL(clicked()),this, SLOT(onSortDesc()));
    connect(ui.cb_sort, SIGNAL(clicked()),this, SLOT(onSortChg()));


    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::Window|/*Qt::WindowSystemMenuHint|*/Qt::WindowTitleHint);
    this->setWindowTitle(tr("Настройки программы"));
    //this->setFixedSize(300,350);
    asc_desc=true;

    /*ui.menu_contents_qlistwidget->clear();
    ui.menu_contents_qlistwidget->insertItem(0,tr("тест1"));
    ui.menu_contents_qlistwidget->insertItem(0,tr("тест2"));
    ui.menu_contents_qlistwidget->insertItem(0,tr("тест3"));*/
    

    QListWidgetItem *other_options = new QListWidgetItem();//ui.menu_contents_qlistwidget
    other_options->setText(tr("основные"));
    other_options->setTextAlignment(Qt::AlignLeft);//AlignHCenter
    //other_options ->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui.menu_contents_qlistwidget->insertItem(1,other_options);
    
    QListWidgetItem *button_options = new QListWidgetItem();//ui.menu_contents_qlistwidget
    button_options->setText(tr("IPwindow"));
    button_options->setTextAlignment(Qt::AlignLeft);
    //button_options->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui.menu_contents_qlistwidget->insertItem(2,button_options);

    QListWidgetItem *terminalDefaultSettings_options = new QListWidgetItem();//ui.menu_contents_qlistwidget
    terminalDefaultSettings_options->setText(tr("новые приборы"));
    terminalDefaultSettings_options->setTextAlignment(Qt::AlignLeft);
    terminalDefaultSettings_options->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui.menu_contents_qlistwidget->insertItem(3,terminalDefaultSettings_options);

    connect(ui.menu_contents_qlistwidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

}

void options::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current) current = previous;
    ui.pagesWidget->setCurrentIndex(ui.menu_contents_qlistwidget->row(current));
}


options::~options()
{

}

void options::iShow()
{
    BaseSettings *bs = BaseSettings::createInstance();

    if(bs->settings->value("Options/solution","1").toUInt()==0)
        ui.cb_solut->setChecked(false);
    else
        ui.cb_solut->setChecked(true);

    if(bs->settings->value("Options/group","0").toUInt()==0)
        ui.cb_group->setChecked(false);
    else
        ui.cb_group->setChecked(true);

/*	if(bs->settings->value("Options/circlecommand","0").toUInt()==0)
        ui.cb_circlecommand->setChecked(false);
    else
        ui.cb_circlecommand->setChecked(true);*/

    if(bs->settings->value("Options/count","0").toUInt()==0)
        ui.cb_count->setChecked(false);
    else
        ui.cb_count->setChecked(true);

    if(bs->settings->value("Options/groupcount","0").toUInt()==0)
        ui.cb_groupcount->setChecked(false);
    else
        ui.cb_groupcount->setChecked(true);

/*	if(bs->settings->value("Options/anycommand","0").toUInt()==0)
        ui.cb_anycommand->setChecked(false);
    else
        ui.cb_anycommand->setChecked(true);*/

    if(bs->settings->value("Options/config","0").toUInt()==0)
        ui.cb_config->setChecked(false);
    else
        ui.cb_config->setChecked(true);

    if(bs->settings->value("Options/sort","0").toUInt()==0)
    {
        ui.cb_sort->setChecked(false);
        ui.rb_asc->setDisabled(true);
        ui.rb_desc->setDisabled(true);
    }
    else
    {
        if(bs->settings->value("Options/asc","1").toUInt()==0)
        {
            ui.rb_asc->setChecked(false);
            ui.rb_desc->setChecked(true);
        }
        else
        {
            ui.rb_asc->setChecked(true);
            ui.rb_desc->setChecked(false);
        }
        ui.rb_asc->setDisabled(false);
        ui.rb_desc->setDisabled(false);
        ui.cb_sort->setChecked(true);
    }

    //-------конфигурация кнопок
    int buttons_config=bs->settings->value("Options/buttons_config","0").toUInt();
    ui.state_radiobutton->setChecked(CHECK_BIT(buttons_config,1));
    ui.state_radiobutton_off->setChecked(!CHECK_BIT(buttons_config,1));
    
    ui.requesttime_radiobutton->setChecked(CHECK_BIT(buttons_config,2));
    ui.requesttime_radiobutton_off->setChecked(!CHECK_BIT(buttons_config,2));
    
    ui.py4huk_radiobutton->setChecked(CHECK_BIT(buttons_config,3));
    ui.py4huk_radiobutton_off->setChecked(!CHECK_BIT(buttons_config,3));
    
    ui.settime_radiobutton->setChecked(CHECK_BIT(buttons_config,4));
    ui.settime_radiobutton_off->setChecked(!CHECK_BIT(buttons_config,4));
    
    ui.par_radiobutton->setChecked(CHECK_BIT(buttons_config,5));
    ui.par_radiobutton_off->setChecked(!CHECK_BIT(buttons_config,5));
    
    ui.service_radiobutton->setChecked(CHECK_BIT(buttons_config,6));
    ui.service_radiobutton_off->setChecked(!CHECK_BIT(buttons_config,6));

    //-------конфигурация кнопок

    bs->freeInst();
    show();
    
}

void options::setpParent(monitor* prt)
{
    parent=(monitor*)prt;
    
}

void options::getOptions()
{
    BaseSettings *bs = BaseSettings::createInstance();

    if(bs->settings->value("Options/solution","1").toUInt()==0)
        parent->option.solution=false;
    else
        parent->option.solution=true;

    if(bs->settings->value("Options/group","0").toUInt()==0)
        parent->option.group=false;
    else
        parent->option.group=true;

    if(bs->settings->value("Options/circlecommand","0").toUInt()==0)
        parent->option.circlecommand=false;
    else
        parent->option.circlecommand=true;

    if(bs->settings->value("Options/count","0").toUInt()==0)
        parent->option.count=false;
    else
        parent->option.count=true;

    if(bs->settings->value("Options/groupcount","0").toUInt()==0)
        parent->option.groupcount=false;
    else
        parent->option.groupcount=true;

    if(bs->settings->value("Options/anycommand","0").toUInt()==0)
        parent->option.anycommand=false;
    else
        parent->option.anycommand=true;
    if(bs->settings->value("Options/config","0").toUInt()==0)
        parent->option.config=false;
    else
        parent->option.config=true;
    if(bs->settings->value("Options/sort","0").toUInt()==0)
        parent->option.sort=false;
    else
    {
        parent->option.sort=true;
        if(bs->settings->value("Options/asc","1").toUInt()==0)
        {
            asc_desc=false;
            parent->option.asc=false;
        }
        else
        {
            asc_desc=true;
            parent->option.asc=true;
        }
        if(bs->settings->value("sort/sort_g","1").toUInt()==0)
            parent->sort_g=false;
        else
            parent->sort_g=true;
        if(bs->settings->value("sort/sort_a","1").toUInt()==0)
            parent->sort_a=false;
        else
            parent->sort_a=true;
        
    }

    bs->freeInst();
}

void options::on_pb_save_clicked()
{
    BaseSettings *bs = BaseSettings::createInstance();
    
    if(ui.cb_solut->isChecked())
    {
        bs->settings->setValue("Options/solution","1");
        parent->option.solution=true;
    }
    else
    {
        bs->settings->setValue("Options/solution","0");
        parent->option.solution=false;
    }
    if(ui.cb_group->isChecked())
    {
        bs->settings->setValue("Options/group","1");
        parent->option.group=true;
    }
    else
    {
        bs->settings->setValue("Options/group","0");
        parent->option.group=false;
    }

    if(ui.cb_count->isChecked())
    {
        bs->settings->setValue("Options/count","1");
        parent->option.count=true;
    }
    else
    {
        bs->settings->setValue("Options/count","0");
        parent->option.count=false;
    }
    if(ui.cb_groupcount->isChecked())
    {
        bs->settings->setValue("Options/groupcount","1");
        parent->option.groupcount=true;
    }
    else
    {
        bs->settings->setValue("Options/groupcount","0");
        parent->option.groupcount=false;
    }
    if(ui.cb_config->isChecked())
    {
        bs->settings->setValue("Options/config","1");
        parent->option.config=true;
    }
    else
    {
        bs->settings->setValue("Options/config","0");
        parent->option.config=false;
    }
    if(ui.cb_sort->isChecked())
    {
        bs->settings->setValue("Options/sort","1");
        parent->option.sort=true;
    }
    else
    {
        bs->settings->setValue("Options/sort","0");
        parent->option.sort=false;
    }

    if(asc_desc)
    {
        bs->settings->setValue("Options/asc","1");
        bs->settings->setValue("sort/sort_g","1");
        bs->settings->setValue("sort/sort_a","1");
        parent->sort_g=true;
        parent->sort_a=true;
        parent->option.asc=true;
    }
    else
    {
        bs->settings->setValue("Options/asc","0");
        bs->settings->setValue("sort/sort_g","0");
        bs->settings->setValue("sort/sort_a","0");
        parent->sort_g=false;
        parent->sort_a=false;
        parent->option.asc=false;
    }

    
//-------конфигурация кнопок
    int buttons_config=0;
    if (ui.state_radiobutton->isChecked()) SET_BIT(buttons_config,1);
    if (ui.requesttime_radiobutton->isChecked()) SET_BIT(buttons_config,2);
    if (ui.py4huk_radiobutton->isChecked()) SET_BIT(buttons_config,3);
    if (ui.settime_radiobutton->isChecked()) SET_BIT(buttons_config,4);
    if (ui.par_radiobutton->isChecked()) SET_BIT(buttons_config,5);
    if (ui.service_radiobutton->isChecked()) SET_BIT(buttons_config,6);
    bs->settings->setValue("Options/buttons_config",QString::number(buttons_config,10));
//-------конфигурация кнопок
//!!! перечитать конфигурацию для того чтобы не перезапускать монитор после применения параметров !!!
    bs->freeInst();
    parent->setAccessUpdate();
    on_pb_close_clicked();
}

void options::on_pb_close_clicked()
{
    hide();
    switch(parent->currMode)
    {
        case MODE_GG:
            parent->mode_sort=0;
            break;
        case MODE_GA:
            parent->mode_sort=1;
            break;
        case MODE_STATE:
            parent->mode_sort=2;
            break;
    }
    parent->setAccessUpdate();
    parent->updateAfterConfig();
    
}
void options::closeEvent(QCloseEvent *event)
{
    switch(parent->currMode)
    {
        case MODE_GG:
            parent->mode_sort=0;
            break;
        case MODE_GA:
            parent->mode_sort=1;
            break;
        case MODE_STATE:
            parent->mode_sort=2;
            break;
    }
    parent->setAccessUpdate();
    parent->updateAfterConfig();
    
    event->accept();   
}

void options::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape)
    {
        hide();
    }
}

void options::onSortAsc()
{
    ui.rb_asc->setChecked(true);
    ui.rb_desc->setChecked(false);
    asc_desc=true;
}

void options::onSortDesc()
{
    ui.rb_asc->setChecked(false);
    ui.rb_desc->setChecked(true);
    asc_desc=false;
}

void options::onSortChg()
{
    if(ui.cb_sort->isChecked())
    {
        ui.rb_asc->setDisabled(false);
        ui.rb_desc->setDisabled(false);
    }
    else
    {
        ui.rb_asc->setDisabled(true);
        ui.rb_desc->setDisabled(true);
    }
}
