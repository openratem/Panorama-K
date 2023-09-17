TEMPLATE = app            # тип исполняемого файла (приложение)
TARGET   = PanoramaK_App # название

#############################################################
#                  подключение модулей QT6                  #
QT += core gui widgets serialport
#CONFIG += console
#############################################################

#BUILD_MODE = builtin

!contains(BUILD_MODE, builtin) {
    DEFINES += PANORAMAK_LIB_LOADER
}

# дирректория куда помещается бинарный файл
include(config/path/path.pri)
DESTDIR = $${DST_PATH}

# оптимизации для release сборки
include(config/optimization/optimization.pri)

# настройка конфигурации компилятора
include(config/configure/configure.pri)

#############################################################
#               подключение заголовочных файлов             #
#############################################################
HEADERS += source/common.h
HEADERS += source/version.h

#############################################################
#              подключение файлов исходного кода            #
#############################################################
SOURCES += source/main.cpp

#############################################################
#               подключение собственных классов             #
#############################################################
HEADERS += source/Kernel.h
SOURCES += source/Kernel.cpp

HEADERS += source/gui/MainWindow.h
SOURCES += source/gui/MainWindow.cpp
FORMS   += source/gui/MainWindow.ui

HEADERS += source/gui/SystemTray/SystemTray.h
SOURCES += source/gui/SystemTray/SystemTray.cpp

HEADERS += source/control/ControlSubject.h
SOURCES += source/control/ControlSubject.cpp

HEADERS += source/control/ControlObserver.h
SOURCES += source/control/ControlObserver.cpp

HEADERS += source/gui/Graphics/Graphics.h
SOURCES += source/gui/Graphics/Graphics.cpp
FORMS   += source/gui/Graphics/Graphics.ui

!contains(BUILD_MODE, builtin) {
    HEADERS += source/service/lib/PanoramaKCommon.h

    HEADERS += source/service/lib/ServiceLib.h
    SOURCES += source/service/lib/ServiceLib.cpp

    HEADERS += source/service/lib/LibLoader.h
    SOURCES += source/service/lib/LibLoader.cpp

    include(../third_party/spdlog/spdlog.pri)
}
else {
    HEADERS += source/service/src/ServiceSrc.h
    SOURCES += source/service/src/ServiceSrc.cpp

    include(../library/PanoramaK_lib/PanoramaK_lib.pri)
}

#############################################################
#                  подключение расширений                   #
#############################################################
include(libs/settings/settings.pri)
include(libs/PowerSwrSeries/PowerSwrSeries.pri)

#############################################################
#               подключение внешних библиотек               #
#############################################################
include(../third_party/qcustomplot/qcustomplot.pri)
include(../third_party/gsl/gsl.pri)
include(../third_party/Indicators/indicators.pri)

#############################################################
#                  подключение файла ресурсов               #
#############################################################
RESOURCES += resource/resource.qrc
