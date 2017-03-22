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

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../

win32:  LIBS += -L$$PWD/../bin/debug \
                -lsim$${POST}

unix:   LIBS += -lboost_thread \
                -lboost_system \
                -L/$$PWD/../bin/ \
                -lsim


#QMAKE_LFLAGS += -lrt
#QMAKE_CXXFLAGS += -lpthread
