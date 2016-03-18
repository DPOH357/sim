TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

win32: INCLUDEPATH += C:/boost_1_58_0/
unix:  INCLUDEPATH += /usr/local/
INCLUDEPATH += /

win32: LIBS += -LC:/boost_1_58_0/lib64-msvc-11.0 \
        -llibboost_thread-vc110-mt-gd-1_58 \
        -llibboost_system-vc110-mt-gd-1_58

unix:  LIBS +=  -L /usr/local/lib \
                -lboost_thread \
                -lboost_system

HEADERS += \
    net/broadcastreceiver.hpp \
    net/broadcastsender.hpp \
    net/gate.hpp \
    tools/tools.h \
    beacon/beacon.h \
    beacon/beaconmessage.h \
    beacon/beaconmode.h

SOURCES += \
    main.cpp \
    beacon/beacon.cpp \
    tools/tools.cpp \
    beacon/beaconmode.cpp
