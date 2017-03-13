#-------------------------------------------------
#
# Project created by QtCreator 2017-03-13T14:58:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:/boost_1_58_0/
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../

LIBS += -LC:/boost_1_58_0/lib64-msvc-11.0 \
    -llibboost_thread-vc110-mt-gd-1_58 \
    -llibboost_system-vc110-mt-gd-1_58 \

LIBS += -L$$PWD/../bin/debug
LIBS += -lsim$${POST}
