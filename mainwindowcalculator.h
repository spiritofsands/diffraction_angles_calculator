#ifndef MAINWINDOWCALCULATOR_H
#define MAINWINDOWCALCULATOR_H

#include <QMainWindow>
#include <QString>

#include "ui_mainWindowCalc.h"

class MainWindowCalculator : public QMainWindow, public Ui::MainWindowCalc
{
Q_OBJECT
public:
    MainWindowCalculator(QWidget *parent = 0);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_CrystalSystemComboBox_currentIndexChanged(const QString &);
    void on_CellEdgeAEdit_textChanged(const QString &);
    void verifyDataForCalcButtons();
    void calculateByHKL();
    void calculateByRanges();
    void about();
	void formulas();
    void chooseFile();
private:
    const int HKLcount;
    const int precision;
	const QString saveDirectoryName;

    QAction *formulasAct;
	QAction *aboutAct;
    QAction *aboutQtAct;

    void createMenu();
    void writeSettings();
    void readSettings();
    QString myDoubleToQString(double);


};

#endif // MAINWINDOWCALCULATOR_H
