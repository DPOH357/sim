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
    base/queue.hpp \
    base/raw_data.h \
    base/timer.h \
    base/tools.h \
    export.h \
    log/log.h \
    net/beacon/beacon.h \
    net/beacon/beaconmode.h \
    net/broadcast.h \
    net/multicast_server.h \
    net/multicast_client.h \
    net/tcp_connection.h \
    net/tcp_receiver.h \
    net/tcp_sender.h \
    base/gate.hpp \
    net/beacon/beaconmessage.hpp


SOURCES += \
    base/raw_data.cpp \
    base/timer.cpp \
    base/tools.cpp \
    log/log.cpp \
    net/beacon/beacon.cpp \
    net/beacon/beaconmode.cpp \
    net/broadcast.cpp \
    net/multicast_server.cpp \
    net/multicast_client.cpp \
    net/tcp_connection.cpp \
    net/tcp_receiver.cpp \
    net/tcp_sender.cpp

