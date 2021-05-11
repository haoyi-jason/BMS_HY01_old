#include "mainwindow.h"
#include "maininfo.h"
#include "stackinfo.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
   // MainInfo w;
    StackInfo w;
    w.show();

    return a.exec();
}
