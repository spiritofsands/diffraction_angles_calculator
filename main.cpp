#include <QApplication>
#include <QMainWindow>
//#include "ui_mainWindow.h"
#include "mainwindowcalculator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindowCalculator *mainWin = new MainWindowCalculator;
    mainWin->show();
    return app.exec();
}
