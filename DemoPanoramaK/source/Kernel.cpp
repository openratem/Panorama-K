#include "Kernel.h"
#include <QApplication>

Kernel::Kernel()
{
    m_subject.attach(m_service.control());
    m_subject.attach(m_mainWindow.control());

    QObject::connect(qApp, &QApplication::commitDataRequest, qApp, [&](auto &val){ endOsSession(val); });
    QObject::connect(qApp, &QApplication::aboutToQuit, qApp, [&](){ onQuit(); });

    restoreSettings();
}

void Kernel::run()
{
    m_mainWindow.show();
}

void Kernel::onQuit()
{
    saveSettings();
}

void Kernel::endOsSession(QSessionManager &)
{
    onQuit();
}

void Kernel::saveSettings()
{
    m_settings.setValue(QStringLiteral("gui"), m_mainWindow.state());

    m_settings.save();
}

void Kernel::restoreSettings()
{
    m_settings.load();

    m_mainWindow.setState(m_settings.value(QStringLiteral("gui")));
}







