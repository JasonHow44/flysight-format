#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T09:09:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlySightFormat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    formatworker.cpp \
    verifyworker.cpp

HEADERS  += mainwindow.h \
    formatworker.h \
    verifyworker.h

FORMS    += mainwindow.ui
