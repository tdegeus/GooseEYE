
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET    = GooseEYE
TEMPLATE  = app

DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += image.cpp\
            qcustomplot.cpp\
            gui_main.cpp\
            gui_mainwindow.cpp

HEADERS  += image.h\
            qcustomplot.h\
            gui_mainwindow.h

FORMS    += gui_mainwindow.ui

CONFIG   += c++14
