INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

QT *= websockets serialport

HEADERS += $$PWD/PanoramaK.h
SOURCES += $$PWD/PanoramaK.cpp

HEADERS += $$PWD/TciTransceiver/TciTransceiver.h
SOURCES += $$PWD/TciTransceiver/TciTransceiver.cpp

HEADERS += $$PWD/TciTransceiver/TciAnalizer/TciAnalizer.h
SOURCES += $$PWD/TciTransceiver/TciAnalizer/TciAnalizer.cpp

HEADERS += $$PWD/CatTransceiver/CatTransceiver.h
SOURCES += $$PWD/CatTransceiver/CatTransceiver.cpp

HEADERS += $$PWD/CatTransceiver/CatAnalizer/CatAnalizer.h
SOURCES += $$PWD/CatTransceiver/CatAnalizer/CatAnalizer.cpp

HEADERS += $$PWD/SwrFilter/SwrFilter.h
SOURCES += $$PWD/SwrFilter/SwrFilter.cpp

RESOURCES += $$PWD/resource/panoramak.qrc

include(libs/tci/tci.pri)
include(libs/OmniRig/OmniRig.pri)
include(../../third_party/spdlog/spdlog.pri)
