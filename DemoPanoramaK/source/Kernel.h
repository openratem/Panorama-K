#pragma once

#include <QSessionManager>
#include <settings.h>
#include "control/ControlSubject.h"
#include "service/Service.h"
#include "gui/MainWindow.h"
#include "common.h"

/**
 * \class Kernel
 * \brief Класс предназначен для управления модулями и настройкой взаимодействия между модулями.
 */
class Kernel final
{
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
    void endOsSession(QSessionManager &session);
    void saveSettings();
    void restoreSettings();

private:
    Service m_service;
    MainWindow m_mainWindow {m_service.series()};
    ControlSubject m_subject;
    ee::settings m_settings {Common::settingsFile()};
};

