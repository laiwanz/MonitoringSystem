#include "Widget.h"
#include "mainwindow.h"
#include <QApplication>
#include "cbl/cbl.h"
#include <QApplication>
#include "CConnectionForSshClient.h"
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w("192.168.55.189", 6379);

    w.show();
   // w.m_PCMaster.init();
   // w.m_PCMaster.masterOpen();
//    Widget m_s;
//    m_s.initUI("192.168.36.220");
//    m_s.show();



    return a.exec();
}
