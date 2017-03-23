
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET    = GooseEYE
TEMPLATE  = app

DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += gui_main.cpp\
            gui_mainwindow.cpp \
            qcustomplot.cpp

HEADERS  += gui_mainwindow.h \
            qcustomplot.h

FORMS    += gui_mainwindow.ui

CONFIG   += c++14
