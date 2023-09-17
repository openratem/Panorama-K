#pragma once

#include "common.h"
#include "control/ControlSubject.h"
#include "gui/MainWindow.h"
#include <QSessionManager>
#include <settings.h>
#ifdef PANORAMAK_LIB_LOADER
#include "service/lib/ServiceLib.h"
#else
#include "service/src/ServiceSrc.h"
#endif

/**
 * \class Kernel
 * \brief Класс предназначен для управления модулями и настройкой взаимодействия между модулями.
 */
class Kernel final {
public:
    /**
     * \brief Конструктор по умолчанию.
     */
    Kernel();

    /**
     * \brief Запуск программы.
     */
    void run();

private:
    void onQuit();
    void endOsSession(QSessionManager& session);
    void saveSettings();
    void restoreSettings();

private:
    Service m_service;
#ifdef PANORAMAK_LIB_LOADER
    MainWindow m_mainWindow {m_service.series(), m_service.getDevices()};
#else
    MainWindow m_mainWindow {m_service.series()};
#endif
    ControlSubject m_subject;
    ee::settings m_settings {Common::settingsFile()};
};
