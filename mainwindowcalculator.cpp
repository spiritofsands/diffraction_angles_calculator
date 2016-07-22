#include <QLocale>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QScrollArea>
#include <QDateTime>
#include <QDebug>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

#include "mainwindowcalculator.h"
#include "angleCalculator.h"

MainWindowCalculator::MainWindowCalculator(QWidget *parent)
    : QMainWindow(parent),
	  HKLcount(3),
      precision(4),
	  saveDirectoryName("Diffraction angles")
{
    setupUi(this);
    createMenu();

    QRegExpValidator *doubleValPtr =
			new QRegExpValidator(QRegExp("[+-]?\\d*[\\.,]?\\d+"), this);
	QRegExpValidator *fileNameValidator =
            new QRegExpValidator(QRegExp("\\w{1,32}[\\s|\\w|-]{0,32}"));
    QIntValidator *intValPtr = new QIntValidator(0, 3600, this);

    lambdaEdit->setValidator( doubleValPtr );
    CellEdgeAEdit->setValidator( doubleValPtr );
    CellEdgeBEdit->setValidator( doubleValPtr );
    CellEdgeCEdit->setValidator( doubleValPtr );
    CellAngleAlphaEdit->setValidator( doubleValPtr );
    CellAngleBetaEdit->setValidator( doubleValPtr );
    CellAngleGammaEdit->setValidator( doubleValPtr );
    hIndexEdit->setValidator( doubleValPtr );
    kIndexEdit->setValidator( doubleValPtr );
    lIndexEdit->setValidator( doubleValPtr );
    resultsCountEdit->setValidator( intValPtr );

    HLeftEdit->setValidator( doubleValPtr );
    HRightEdit->setValidator( doubleValPtr );
    KLeftEdit->setValidator( doubleValPtr );
    KRightEdit->setValidator( doubleValPtr );
    LLeftEdit->setValidator( doubleValPtr );
    LRightEdit->setValidator( doubleValPtr );

    thetaStepEdit->setValidator( doubleValPtr );
	expositionEdit->setValidator( doubleValPtr );
	setupTimeEdit->setValidator( doubleValPtr );
	outFileNameEdit->setValidator(fileNameValidator);
    QString fileName = QDateTime::currentDateTime().toString("hh-mm dd-MM-yyyy");
    outFileNameEdit->setText(fileName);

    calculateHKLbutton->setEnabled(false);
    calculateRangesButton->setEnabled(false);

    readSettings();
    verifyDataForCalcButtons();

    connect(calculateHKLbutton, SIGNAL(clicked()), this, SLOT(calculateByHKL()));
    connect(calculateRangesButton, SIGNAL(clicked()), this, SLOT(calculateByRanges()));
    connect(changeDirButton, SIGNAL(clicked()), this, SLOT(chooseFile()));

    connect(lambdaEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(CellEdgeAEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(CellEdgeBEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(CellEdgeCEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(CellAngleAlphaEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(CellAngleBetaEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(CellAngleGammaEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));

    connect(hIndexEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(kIndexEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(lIndexEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));

    connect(HLeftEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(HRightEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(KLeftEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(KRightEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(LLeftEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(LRightEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));

    connect(resultsCountEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
    connect(outFileNameEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyDataForCalcButtons()));
}

void MainWindowCalculator::closeEvent(QCloseEvent */*event*/)
{
    writeSettings();
}

void MainWindowCalculator::chooseFile()
{
	QString initialFileName = outFileNameEdit->text();

	if (!QDir(QDir::homePath() + '/' + saveDirectoryName).exists())
		QDir().mkdir(QDir::homePath() + '/' + saveDirectoryName);

	if (initialFileName.isEmpty()){
		initialFileName = QDir::homePath() +
				'/' + saveDirectoryName +
				'/' + QDateTime::currentDateTime().toString("hh-mm dd-MM-yyyy");
	}
	else {
		initialFileName = QDir::homePath() +
				'/' + saveDirectoryName +
				'/' + initialFileName;
	}

    QString fileName = QFileDialog::getSaveFileName(this,
                                "Choose data file",
								initialFileName,
								"Text files (*.txt)");

	if (fileName.isEmpty())
		fileName = initialFileName;

	outFileNameEdit->setText( QFileInfo(fileName).completeBaseName() );
}

void MainWindowCalculator::writeSettings()
{
    QSettings settings("SpiritOfSands", "Diffraction angles calculator");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("lambdaEdit", lambdaEdit->text());
    settings.setValue("CrystalSystemComboBox", CrystalSystemComboBox->currentIndex());
    settings.setValue("CellEdgeAEdit", CellEdgeAEdit->text());
    settings.setValue("CellEdgeBEdit", CellEdgeBEdit->text());
    settings.setValue("CellEdgeCEdit", CellEdgeCEdit->text());
    settings.setValue("CellAngleAlphaEdit", CellAngleAlphaEdit->text());
    settings.setValue("CellAngleBetaEdit", CellAngleBetaEdit->text());
    settings.setValue("CellAngleGammaEdit", CellAngleGammaEdit->text());
    settings.setValue("tabWidget", tabWidget->currentIndex());

    settings.setValue("hIndexEdit", hIndexEdit->text());
    settings.setValue("kIndexEdit", kIndexEdit->text());
    settings.setValue("lIndexEdit", lIndexEdit->text());

    settings.setValue("HLeftEdit", HLeftEdit->text());
    settings.setValue("HRightEdit", HRightEdit->text());
    settings.setValue("KLeftEdit", KLeftEdit->text());
    settings.setValue("KRightEdit", KRightEdit->text());
    settings.setValue("LLeftEdit", LLeftEdit->text());
    settings.setValue("LRightEdit", LRightEdit->text());

    settings.setValue("thetaStepEdit", thetaStepEdit->text());
	settings.setValue("expositionEdit", expositionEdit->text());
	settings.setValue("setupTimeEdit", setupTimeEdit->text());
    settings.setValue("resultsCountEdit", resultsCountEdit->text());
}

void MainWindowCalculator::readSettings()
{
    QSettings settings("SpiritOfSands", "Diffraction angles calculator");

    restoreGeometry(settings.value("geometry").toByteArray());

    QString lambda = settings.value("lambdaEdit", "1,65791").toString();
    lambdaEdit->setText(lambda);

    int CrystalSystemComboBoxIndex = settings.value("CrystalSystemComboBox", "0").toInt();
    CrystalSystemComboBox->setCurrentIndex(CrystalSystemComboBoxIndex);

    QString a = settings.value("CellEdgeAEdit", "8,404").toString();
    CellEdgeAEdit->setText(a);
    QString b = settings.value("CellEdgeBEdit", "8,404").toString();
    CellEdgeBEdit->setText(b);
    QString c = settings.value("CellEdgeCEdit", "21,183").toString();
    CellEdgeCEdit->setText(c);

    QString alpha = settings.value("CellAngleAlphaEdit", "90").toString();
    CellAngleAlphaEdit->setText(alpha);
    QString beta = settings.value("CellAngleBetaEdit", "90").toString();
    CellAngleBetaEdit->setText(beta);
    QString gamma = settings.value("CellAngleGammaEdit", "120").toString();
    CellAngleGammaEdit->setText(gamma);

    int tabIndex = settings.value("tabWidget", "0").toInt();
    tabWidget->setCurrentIndex(tabIndex);

    QString h = settings.value("hIndexEdit", "4").toString();
    hIndexEdit->setText(h);
    QString k = settings.value("kIndexEdit", "1").toString();
    kIndexEdit->setText(k);
    QString l = settings.value("lIndexEdit", "6").toString();
    lIndexEdit->setText(l);

    QString HLeft = settings.value("HLeftEdit", "1").toString();
    HLeftEdit->setText(HLeft);
    QString HRight = settings.value("HRightEdit", "3").toString();
    HRightEdit->setText(HRight);

    QString KLeft = settings.value("KLeftEdit", "1").toString();
    KLeftEdit->setText(KLeft);
    QString KRight = settings.value("KRightEdit", "3").toString();
    KRightEdit->setText(KRight);

    QString LLeft = settings.value("LLeftEdit", "0").toString();
    LLeftEdit->setText(LLeft);
    QString LRight = settings.value("LRightEdit", "0").toString();
    LRightEdit->setText(LRight);

	QString thetaStep = settings.value("thetaStepEdit", "0,07").toString();
	thetaStepEdit->setText(thetaStep);
	QString exposition = settings.value("expositionEdit", "3").toString();
	expositionEdit->setText(exposition);
	QString setupTime = settings.value("setupTimeEdit", "30").toString();
	setupTimeEdit->setText(setupTime);

    QString resCount = settings.value("resultsCountEdit", "20").toString();
    resultsCountEdit->setText(resCount);
}

void MainWindowCalculator::createMenu()
{
	formulasAct = new QAction("&Formulas", this);
    aboutAct = new QAction("A&bout", this);
    aboutQtAct = new QAction("About Q&t", this);

	connect(formulasAct, SIGNAL(triggered()), this, SLOT(formulas()));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAct, SIGNAL(triggered()), QApplication::instance(), SLOT(aboutQt()));

	Ui::MainWindowCalc::menuBar->addAction(formulasAct);
	Ui::MainWindowCalc::menuBar->addSeparator();
    Ui::MainWindowCalc::menuBar->addAction(aboutAct);
    Ui::MainWindowCalc::menuBar->addAction(aboutQtAct);
}

void MainWindowCalculator::formulas()
{
	QDialog *Dialog = new QDialog(this);
	Dialog->setWindowTitle("Formulas");
	Dialog->resize(692, 384);
	Dialog->setWindowFlags(Qt::Window);
	Dialog->setFixedWidth(Dialog->width());

	verticalLayout = new QVBoxLayout(Dialog);

	QScrollArea *scrollArea = new QScrollArea(Dialog);
	scrollArea->setWidgetResizable(true);

	QWidget *scrollAreaWidgetContents = new QWidget();

	QVBoxLayout *verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);

	QLabel *label = new QLabel(scrollAreaWidgetContents);
	label->setPixmap(QPixmap(":/theta.PNG"));

	verticalLayout_2->addWidget(label);

	QLabel *label_2 = new QLabel(scrollAreaWidgetContents);
	label_2->setPixmap(QPixmap(":/hi.PNG"));

	verticalLayout_2->addWidget(label_2);

	QLabel *label_3 = new QLabel(scrollAreaWidgetContents);
	label_3->setPixmap(QPixmap(":/fi.PNG"));

	verticalLayout_2->addWidget(label_3);

	scrollArea->setWidget(scrollAreaWidgetContents);

	verticalLayout->addWidget(scrollArea);

	Dialog->show();

}

void MainWindowCalculator::about()
{
    QMessageBox::about(this, "About diffraction angles calculator",
                        "<h2>Diffraction angles calculator</h2>"
                        "<p>Created by Kostya Fursenko; 2016"
                        "<p><a href=\"mailto:spiritofsands@gmail.com\">spiritofsands@gmail.com</a>"
                        "<p>This is a small application that "
						"can calculate 2Theta, Chi and Phi angles "
                        "with set of HKL or range of them.");
}

void MainWindowCalculator::verifyDataForCalcButtons()
{
    static bool lambdaIsOK,
                CellEdgeAIsOk,
                CellEdgeBIsOk,
                CellEdgeCIsOk,
                CellAngleAlphaIsOk,
                CellAngleBetaIsOk,
                CellAngleGammaIsOk,
                hIndexIsOk,
                kIndexIsOk,
                lIndexIsOk,

                HLeftIsOk,
                HRightIsOk,
                KLeftIsOk,
                KRightIsOk,
                LLeftIsOk,
                LRightIsOk,

                resultsCountIsOk,
                outFileNameIsOk,
                upperInputIsCorrect,
                HKLinputIsCorrect,
                rangesInputIsCorrect;

    //inititlize to false
    lambdaIsOK = CellEdgeAIsOk = CellEdgeBIsOk = CellEdgeCIsOk =
            CellAngleAlphaIsOk = CellAngleBetaIsOk = CellAngleGammaIsOk =
            hIndexIsOk = kIndexIsOk = lIndexIsOk =
            HLeftIsOk = HRightIsOk =
            KLeftIsOk = KRightIsOk =
            LLeftIsOk = LRightIsOk =
            resultsCountIsOk =
            outFileNameIsOk = false;

    if (lambdaEdit->hasAcceptableInput())
        lambdaIsOK = true;
    if (CellEdgeAEdit->hasAcceptableInput())
        CellEdgeAIsOk = true;
    if (CellEdgeBEdit->hasAcceptableInput())
        CellEdgeBIsOk = true;
    if (CellEdgeCEdit->hasAcceptableInput())
        CellEdgeCIsOk = true;
    if (CellAngleAlphaEdit->hasAcceptableInput())
        CellAngleAlphaIsOk = true;
    if (CellAngleBetaEdit->hasAcceptableInput())
        CellAngleBetaIsOk = true;
    if (CellAngleGammaEdit->hasAcceptableInput())
        CellAngleGammaIsOk = true;

    if (hIndexEdit->hasAcceptableInput())
        hIndexIsOk = true;
    if (kIndexEdit->hasAcceptableInput())
        kIndexIsOk = true;
    if (lIndexEdit->hasAcceptableInput())
        lIndexIsOk = true;

    if (HLeftEdit->hasAcceptableInput())
        HLeftIsOk = true;
    if (HRightEdit->hasAcceptableInput())
        HRightIsOk = true;

    if (KLeftEdit->hasAcceptableInput())
        KLeftIsOk = true;
    if (KRightEdit->hasAcceptableInput())
        KRightIsOk = true;

    if (LLeftEdit->hasAcceptableInput())
        LLeftIsOk = true;
    if (LRightEdit->hasAcceptableInput())
        LRightIsOk = true;

    if (resultsCountEdit->hasAcceptableInput())
        resultsCountIsOk = true;
	if (outFileNameEdit->hasAcceptableInput())
        outFileNameIsOk = true;

    upperInputIsCorrect = lambdaIsOK &&
            CellEdgeAIsOk && CellEdgeBIsOk && CellEdgeCIsOk &&
            CellAngleAlphaIsOk && CellAngleBetaIsOk && CellAngleGammaIsOk;

    HKLinputIsCorrect = hIndexIsOk && kIndexIsOk && lIndexIsOk;
    rangesInputIsCorrect = HLeftIsOk && HRightIsOk &&
            KLeftIsOk && KRightIsOk &&
            LLeftIsOk && LRightIsOk &&
            resultsCountIsOk && outFileNameIsOk;

    calculateHKLbutton->setEnabled( upperInputIsCorrect && HKLinputIsCorrect );
    calculateRangesButton->setEnabled( upperInputIsCorrect && rangesInputIsCorrect );
}

void MainWindowCalculator::calculateByHKL()
{
    double lambda = lambdaEdit->text().replace(",", ".").toDouble(),
        //lattice distances in angstrem
        a = CellEdgeAEdit->text().replace(",", ".").toDouble(),
        b = CellEdgeBEdit->text().replace(",", ".").toDouble(),
        c = CellEdgeCEdit->text().replace(",", ".").toDouble(),
        //lattice angles in degrees
        alpha = CellAngleAlphaEdit->text().replace(",", ".").toDouble(),
        beta = CellAngleBetaEdit->text().replace(",", ".").toDouble(),
        gamma = CellAngleGammaEdit->text().replace(",", ".").toDouble(),
        //hkl
        h = hIndexEdit->text().replace(",", ".").toDouble(),
        k = kIndexEdit->text().replace(",", ".").toDouble(),
        l = lIndexEdit->text().replace(",", ".").toDouble();

    try {
        AngleCalculator calc( lambda,
                              a, b, c,
                              alpha, beta, gamma );
        calc.calculateResultingAngles(h, k, l);

        twoTethaEdit->setText( myDoubleToQString(calc.getTwoTetha()) );
        HiEdit->setText( myDoubleToQString(calc.getXi()) );
        FiEdit->setText( myDoubleToQString(calc.getFi()) );
    }
    catch ( exception &e )
    {
        QMessageBox msgBox(QMessageBox::Warning,
                           "Error",
                           QString("Calculation stopped.\n\nException occured:\n") + e.what(),
                           QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindowCalculator::calculateByRanges()
{
    double lambda = lambdaEdit->text().replace(",", ".").toDouble(),
        //lattice distances in angstrem
        a = CellEdgeAEdit->text().replace(",", ".").toDouble(),
        b = CellEdgeBEdit->text().replace(",", ".").toDouble(),
        c = CellEdgeCEdit->text().replace(",", ".").toDouble(),
        //lattice angles in degrees
        /*
            L (c) ^
                  |
                  |
                  | alpha  K (b)
             beta |________>
                 /
                / gamma
               /
              v H (a)
        */
        alpha = CellAngleAlphaEdit->text().replace(",", ".").toDouble(),
        beta = CellAngleBetaEdit->text().replace(",", ".").toDouble(),
        gamma = CellAngleGammaEdit->text().replace(",", ".").toDouble(),
        //ranges and etc
        HLeft = HLeftEdit->text().replace(",", ".").toDouble(),
        HRight = HRightEdit->text().replace(",", ".").toDouble(),
        KLeft = KLeftEdit->text().replace(",", ".").toDouble(),
        KRight = KRightEdit->text().replace(",", ".").toDouble(),
        LLeft = LLeftEdit->text().replace(",", ".").toDouble(),
        LRight = LRightEdit->text().replace(",", ".").toDouble(),

        thetaStep = thetaStepEdit->text().replace(",", ".").toDouble(),
        setupTime = setupTimeEdit->text().replace(",", ".").toDouble(),
        exposition = expositionEdit->text().replace(",", ".").toDouble();

    int resultsCount = resultsCountEdit->text().replace(",", ".").toInt();

    try {
          ///connecting part
          /*
                                    sigma = angle between x & y
                     Z ^
                       |            fi = arctg( (x/y - cos(sigma)) / sin(sigma) )
                       |
                       |        Y   c = y / cos(fi)
                       |________>
                      / \/fi
                     /   \
                    /     \
                   v X     v C


                L ^                       y ^
                  |                         |
                  |                         |
                  |        K                |         x
                  |________>      = >       |_________>
                 /
                /
               /
              v H
        */
        double xLeft, xRight,
                yLeft, yRight;
        double sigma;
        double fiLeft, fiRight;

        bool HisZero = (HLeft == 0.0) && (HRight == 0.0),
             KisZero = (KLeft == 0.0) && (KRight == 0.0),
             LisZero = (LLeft == 0.0) && (LRight == 0.0);

        if (HisZero)
        {
            xLeft = KLeft;
            xRight = KRight;

            yLeft = LLeft;
            yRight = LRight;

            if (alpha != 90)
                throw logic_error("Calculations are not implemented"
                                  " for angles != 90.");
        }
        else if (KisZero)
        {
            xLeft = HLeft;
            xRight = HRight;

            yLeft = LLeft;
            yRight = LRight;

            if (beta != 90)
                throw logic_error("Calculations are not implemented"
                                  " for angles != 90.");
        }
        else if (LisZero)
        {
            xLeft = HLeft;
            xRight = HRight;

            yLeft = KLeft;
            yRight = KRight;

            if (gamma != 90)
                throw logic_error("Calculations are not implemented"
                                  " for angles != 90.");
        }
        else //all indexes are non zero
        {
            if (alpha != 90 || beta != 90)
                throw logic_error("Calculations are not implemented"
                                  " for angles != 90.");

            sigma = gamma;
            fiLeft = atan( (HLeft/KLeft - cos(sigma)) / sin(sigma) );
            fiRight = atan( (HRight/KRight - cos(sigma)) / sin(sigma) );

            xLeft = KLeft / cos(fiLeft);
            xRight = KRight / cos(fiRight);

            yLeft = LLeft;
            yRight = LRight;
        }

        ///calc part

        AngleCalculator calc( lambda,
                              a, b, c,
                              alpha, beta, gamma );
        calc.buildPoints(xLeft, xRight,
                         yLeft, yRight,
                         resultsCount);

        my2dVector indices;
        indices = calc.getVectorOfCoords();

        int experimentTime = ceil(setupTime * (resultsCount-1));

        //open file
        QString outFileName = outFileNameEdit->text();
        if (!outFileName.endsWith(".txt")) {
            outFileName = QDir::homePath() + '/'
                    + saveDirectoryName + '/' + outFileName;
            outFileName.append(".txt");
        }
        QFile outFile(outFileName);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
            throw logic_error("Can't access file.");
        QTextStream out(&outFile);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(precision);
        //end of opening of file

        out << "Lambda = " << lambda
            << "\na = " << a
            << "\nb = " << b
            << "\nc = " << c
            << "\nalpha = " << alpha
            << "\nbeta = " << beta
            << "\ngamma = " << gamma << "\n";
        out << "\nH range: "
            << HLeft << " .. " << HRight;
        out << "\nK range: "
            << KLeft << " .. " << KRight;
        out << "\nL range: "
            << LLeft << " .. " << LRight;

        out << "\n\nCount of results: " << resultsCount << "\n\n";

		double thetaPath,
				firstTheta,
				secondTheta;
		int stepsOfTheta;

        double currentHleft = 0.0, currentHright = 0.0,
                currentKleft = 0.0, currentKright = 0.0,
                currentLleft = 0.0, currentLright = 0.0;

        for (size_t i = 0; i < indices.size(); i++)
        {
            if (HisZero)
            {
                currentKleft = indices[i][xFirst];
                currentKright = indices[i][xSecond];

                currentLleft = indices[i][yFirst];
                currentLright = indices[i][ySecond];
            }
            else if (KisZero)
            {
                currentHleft = indices[i][xFirst];
                currentHright = indices[i][xSecond];

                currentLleft = indices[i][yFirst];
                currentLright = indices[i][ySecond];
            }
            else if (LisZero)
            {
                currentHleft = indices[i][xFirst];
                currentHright = indices[i][xSecond];

                currentKleft = indices[i][yFirst];
                currentKright = indices[i][ySecond];
            }
            else {
                currentHleft = indices[i][xFirst] * cos(sigma - fiLeft);
                currentHright = indices[i][xSecond] * cos(sigma - fiRight);

                currentKleft = indices[i][xFirst] * cos(fiLeft);
                currentKright = indices[i][xSecond] * cos(fiRight);

                currentLleft = indices[i][yFirst];
                currentLright = indices[i][ySecond];
            }

            out << "=========================================="
                   "\nNumber: " << i+1;
            out << "\n#1:\n"
                   "\tHKL: (" << currentHleft << ' ' << currentKleft << ' ' << currentLleft << ")\n";
            calc.calculateResultingAngles(currentHleft, currentKleft, currentLleft);
            out << "\t2Tetha: " << myDoubleToQString(calc.getTwoTetha())
                << "\n\t    Hi: " << myDoubleToQString(calc.getXi())
                << "\n\t    Fi: " << myDoubleToQString(calc.getFi()) << "\n";

			firstTheta = calc.getTwoTetha();

            out << "\n#2:\n"
                   "\tHKL: (" << currentHright << ' ' << currentKright << ' ' << currentLright << ")\n";
            calc.calculateResultingAngles(currentHright, currentKright, currentLright);
            out << "\t2Tetha: " << myDoubleToQString(calc.getTwoTetha())
                << "\n\t    Hi: " << myDoubleToQString(calc.getXi())
                << "\n\t    Fi: " << myDoubleToQString(calc.getFi()) << "\n\n";

			secondTheta = calc.getTwoTetha();

			thetaPath = fabs(firstTheta - secondTheta);

			stepsOfTheta = ceil (thetaPath / thetaStep);
			if (stepsOfTheta == 0)
				stepsOfTheta = 1;

			experimentTime += stepsOfTheta * exposition;
        }

        QString timeStr("\n\nExperiment time is about:\n\t");
        timeStr += QDateTime::fromTime_t(experimentTime).toUTC().toString("hh:mm:ss");
		out << "=========================================="
			<< timeStr << "\n";

        outFile.close();

        QMessageBox msgBox(QMessageBox::Information,
                           "Status",
						   "Finished." + timeStr,
                           QMessageBox::Ok);
        QAbstractButton* buttonOpenFilePtr = msgBox.addButton("Open file", QMessageBox::ActionRole);
        QAbstractButton* buttonOpenFolderPtr = msgBox.addButton("Open folder", QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton()==buttonOpenFilePtr)
            QDesktopServices::openUrl(QUrl::fromLocalFile(outFileName));
        else if (msgBox.clickedButton()==buttonOpenFolderPtr)
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(outFileName).absolutePath()));
    }
    catch ( exception &e )
    {
        QMessageBox msgBox(QMessageBox::Warning,
                           "Error",
                           QString("Calculation stopped.\n\nException occured:\n") + e.what(),
                           QMessageBox::Ok);
        msgBox.exec();
    }


}

void MainWindowCalculator::on_CellEdgeAEdit_textChanged(const QString &a)
{
    if (!CellEdgeBEdit->isEnabled())
        CellEdgeBEdit->setText(a);
    if (!CellEdgeCEdit->isEnabled())
        CellEdgeCEdit->setText(a);
}

void MainWindowCalculator::on_CrystalSystemComboBox_currentIndexChanged(const QString &crystalSystem)
{
    if (crystalSystem == "triclinic") {
        CellAngleAlphaEdit->setEnabled(true);
        CellAngleBetaEdit->setEnabled(true);
        CellAngleGammaEdit->setEnabled(true);

        CellEdgeBEdit->setEnabled(true);
        CellEdgeCEdit->setEnabled(true);

    } else if (crystalSystem == "monoclinic") {
        CellAngleAlphaEdit->setEnabled(false);
        CellAngleAlphaEdit->setText("90");
        CellAngleBetaEdit->setEnabled(true);
        CellAngleGammaEdit->setEnabled(false);
        CellAngleGammaEdit->setText("90");

        CellEdgeBEdit->setEnabled(true);
        CellEdgeCEdit->setEnabled(true);

    } else if (crystalSystem == "orthorhombic") {
        CellAngleAlphaEdit->setEnabled(false);
        CellAngleAlphaEdit->setText("90");
        CellAngleBetaEdit->setEnabled(false);
        CellAngleBetaEdit->setText("90");
        CellAngleGammaEdit->setEnabled(false);
        CellAngleGammaEdit->setText("90");

        CellEdgeBEdit->setEnabled(true);
        CellEdgeCEdit->setEnabled(true);

    } else if (crystalSystem == "tetragonal") {
        CellAngleAlphaEdit->setEnabled(false);
        CellAngleAlphaEdit->setText("90");
        CellAngleBetaEdit->setEnabled(false);
        CellAngleBetaEdit->setText("90");
        CellAngleGammaEdit->setEnabled(false);
        CellAngleGammaEdit->setText("90");

        CellEdgeBEdit->setEnabled(false);
        CellEdgeBEdit->setText(CellEdgeAEdit->text());
        CellEdgeCEdit->setEnabled(true);

    } else if (crystalSystem == "trigonal, hexagonal") {
        CellAngleAlphaEdit->setEnabled(false);
        CellAngleAlphaEdit->setText("90");
        CellAngleBetaEdit->setEnabled(false);
        CellAngleBetaEdit->setText("90");
        CellAngleGammaEdit->setEnabled(false);
        CellAngleGammaEdit->setText("120");

        CellEdgeBEdit->setEnabled(false);
        CellEdgeBEdit->setText(CellEdgeAEdit->text());
        CellEdgeCEdit->setEnabled(true);

    } else if (crystalSystem == "cubic") {
        CellAngleAlphaEdit->setEnabled(false);
        CellAngleAlphaEdit->setText("90");
        CellAngleBetaEdit->setEnabled(false);
        CellAngleBetaEdit->setText("90");
        CellAngleGammaEdit->setEnabled(false);
        CellAngleGammaEdit->setText("90");

        CellEdgeBEdit->setEnabled(false);
        CellEdgeBEdit->setText(CellEdgeAEdit->text());
        CellEdgeCEdit->setEnabled(false);
        CellEdgeCEdit->setText(CellEdgeAEdit->text());

    }
}

QString MainWindowCalculator::myDoubleToQString(double n)
{
    static const QLocale loc;
    return std::isnan(n) ? "Any" : loc.toString(n, 'f', precision);
}
