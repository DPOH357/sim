TEMPLATE = lib
TARGET = sim

DEFINES += SIM_LIB

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++14

win32: INCLUDEPATH += C:/boost_1_58_0/
unix:  INCLUDEPATH += /usr/local/
INCLUDEPATH += $$PWD

win32: LIBS +=  -LC:/boost_1_58_0/lib64-msvc-11.0 \
                -llibboost_thread-vc110-mt-gd-1_58 \
                -llibboost_system-vc110-mt-gd-1_58

unix:  LIBS +=  -L /usr/local/lib \
                -lboost_thread \
                -lboost_system

HEADERS += \
    export.h \
    net/beacon/beacon.h \
    net/beacon/beaconmessage.h \
    net/beacon/beaconmode.h \
    net/broadcastreceiver.hpp \
    net/broadcastsender.hpp \
    net/gate.hpp \
    base/timer.h \
    base/raw_data.h \
    base/tools.h \
    net/broadcast.h \
    net/tcp_connection.h \
    base/queue.hpp \
    net/tcp_receiver.h

SOURCES += \
    net/beacon/beacon.cpp \
    net/beacon/beaconmode.cpp \
    net/tcp_connection.cpp \
    base/timer.cpp \
    base/raw_data.cpp \
    base/tools.cpp \
    net/broadcast.cpp \
    net/tcp_receiver.cpp
