TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++14

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = simchat

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

HEADERS += \
    mainwindow.h \
    chat_manager.h \
    userlistdetecor.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    userlistdetecor.cpp

FORMS += \
    mainwindow.ui
