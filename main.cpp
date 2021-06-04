#include "mainwindow.h"
#include "maininfo.h"
#include "stackinfo.h"
#include <QApplication>
#include "collectorview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qputenv("QT_IM_MODULE",QByteArray("qtvirtualkeyboard"));
//    MainWindow w;
   // MainInfo w;
//    StackInfo w;
    CollectorView w;
    w.show();

    return a.exec();
}
