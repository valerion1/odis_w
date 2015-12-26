#include <QtWidgets/QApplication>
#include "monitor.h"
#include <QTextCodec>  
#include <QtPlugin>
#include <QTranslator>

/*QT_BEGIN_NAMESPACE
Q_IMPORT_PLUGIN(qsqlmysql)
QT_END_NAMESPACE*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator *qt_translator = new QTranslator; 
    if ( qt_translator->load( "./Resources/qt_ru.qm" ) ) 
    {
        a.installTranslator( qt_translator );
    }

        // old QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    monitor w;
    a.connect(&w, SIGNAL(needExit()), &a, SLOT(quit()));
    //a.addLibraryPath(a.applicationDirPath() + "/sqldrivers");
    if (w.makeConnection())
    {
        w.iShow();
        a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
        return a.exec();
    }else{
        return -1;
    }
}
