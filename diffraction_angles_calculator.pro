QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE    = app
FORMS       = \
    mainWindowCalc.ui

SOURCES += \
    mainwindowcalculator.cpp \
    main.cpp \
    angleCalculator.cpp

HEADERS += \
    mainwindowcalculator.h \
    angleCalculator.h

RESOURCES += \
    res.qrc
