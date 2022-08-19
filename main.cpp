#include "mainwindow.h"
#include "maininfo.h"
#include "stackinfo.h"
#include <QApplication>
#include "collectorview.h"
#include "frmstackconfig.h".h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
   // MainInfo w;
//    StackInfo w;
    if(argc > 1){
        QTextStream out(stdout);
        out << "22032302\n";
        return 0;
    }
    else{
//        CollectorView w;
        frmStackConfig w("");
        w.show();
        return a.exec();
    }

}
