#-------------------------------------------------
#
# Project created by QtCreator 2020-04-04T20:42:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = modbus
TEMPLATE = app

CONFIG += C++11
INCLUDEPATH += /usr/local/include/modbus
LIBS += -L/usr/local/lib -lmodbus


SOURCES += main.cpp\
        mainwindow.cpp \
    modbusobject.cpp \
    config.cpp \
    configwidget.cpp \
    mainwidget.cpp

HEADERS  += mainwindow.h \
    modbusobject.h \
    config.h \
    configwidget.h \
    mainwidget.h

FORMS    += mainwindow.ui \
    configwidget.ui \
    mainwidget.ui
