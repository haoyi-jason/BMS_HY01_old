#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "windows.h"
//#include "VCI_CAN.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    _VCI_CAN_PARAM param;
//    BYTE Mod_CfgData[512];

//    Mod_CfgData[0] = Mod_CfgData[1] = 0;
//    VCI_Set_MOD_Ex(Mod_CfgData);
}

MainWindow::~MainWindow()
{
    delete ui;
}
