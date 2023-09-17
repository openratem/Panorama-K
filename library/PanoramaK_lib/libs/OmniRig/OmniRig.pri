INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

HEADERS += $$PWD/RigAbstract.h
HEADERS += $$PWD/rig/control/ringbufferevent.h
HEADERS += $$PWD/rig/control/queueevent.h

HEADERS += $$PWD/omnirig.h
SOURCES += $$PWD/omnirig.cpp

HEADERS += $$PWD/rig/Rig.h
SOURCES += $$PWD/rig/Rig.cpp

HEADERS += $$PWD/rig/parser/RigParser.h
SOURCES += $$PWD/rig/parser/RigParser.cpp

HEADERS += $$PWD/rig/serialport/RigSerialPort.h
SOURCES += $$PWD/rig/serialport/RigSerialPort.cpp

HEADERS += $$PWD/rig/RigSerialPortParam.h
SOURCES += $$PWD/rig/RigSerialPortParam.cpp

HEADERS += $$PWD/rig/control/RigControl.h
SOURCES += $$PWD/rig/control/RigControl.cpp

HEADERS += $$PWD/rig/converter/CmdAbstract.h
HEADERS += $$PWD/rig/converter/cmd/RigCmd.h
SOURCES += $$PWD/rig/converter/cmd/RigCmd.cpp
