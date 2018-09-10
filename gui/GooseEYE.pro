
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET    = GooseEYE
TEMPLATE  = app

DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += qcustomplot.cpp\
            main.cpp\
            mainwindow.cpp

HEADERS  += qcustomplot.h\
            mainwindow.h

FORMS    += mainwindow.ui

CONFIG   += c++14

INCLUDEPATH += "include"
INCLUDEPATH += include
