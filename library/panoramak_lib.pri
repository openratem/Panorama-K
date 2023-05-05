INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

QT *= websockets serialport

HEADERS += $$PWD/PanoramaK.h \
    $$PWD/SwrFilter/SwrFilter.h \
    $$PWD/TciTransceiver/TciAnalizer/TciAnalizer.h
SOURCES += $$PWD/PanoramaK.cpp \
    $$PWD/SwrFilter/SwrFilter.cpp \
    $$PWD/TciTransceiver/TciAnalizer/TciAnalizer.cpp

HEADERS += $$PWD/TciTransceiver/TciTransceiver.h
SOURCES += $$PWD/TciTransceiver/TciTransceiver.cpp

include(libs/tci/tci.pri)
include(../third_party/spdlog/spdlog.pri)
