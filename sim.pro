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
    base/gate.hpp \
    net/beacon/beaconmessage.hpp \
    net/server/server.h \
    net/unicast_connection.h \
    net/unicast_receiver.h \
    net/unicast_sender.h

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
    net/server/server.cpp \
    net/unicast_connection.cpp \
    net/unicast_receiver.cpp \
    net/unicast_sender.cpp
