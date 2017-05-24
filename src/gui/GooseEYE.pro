
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET    = GooseEYE
TEMPLATE  = app

DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += ../core/image.cpp\
            qcustomplot.cpp\
            main.cpp\
            mainwindow.cpp

HEADERS  += ../core/image.h\
            cppcolormap/cppcolormap.h\
            qcustomplot.h\
            mainwindow.h

FORMS    += mainwindow.ui

CONFIG   += c++14
