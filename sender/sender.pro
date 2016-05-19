#-------------------------------------------------
#
# Project created by QtCreator 2016-05-19T15:34:51
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = sender
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32: INCLUDEPATH += C:/boost_1_58_0/
unix:  INCLUDEPATH += /usr/local/
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../

win32: LIBS += -LC:/boost_1_58_0/lib64-msvc-11.0 \
        -llibboost_thread-vc110-mt-gd-1_58 \
        -llibboost_system-vc110-mt-gd-1_58

unix:  LIBS +=  -L /usr/local/lib \
                -lboost_thread \
                -lboost_system

LIBS += -L$$PWD/../bin/debug
LIBS += -lsim$${POST}


SOURCES += main.cpp
