#pragma once

#include <string_view>

#include <QDir>
#include <QString>
#include <QStringList>
#include <QStandardPaths>
#include <QGuiApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QSurfaceFormat>
#include <include/spdlog/fmt/fmt.h>

#include "version.h"


///  \brief Название программного обеспечения.
static constexpr std::string_view ApplicationName("DemoPanorama-K");

///  \brief Название компании.
static constexpr std::string_view OrganizationName("Ratem");

///  \brief Официальный сайт компании.
static constexpr std::string_view OrganizationDomain("ratem.ru");

///  \brief Версия программы.
static const std::string ApplicatioVersion = fmt::format("{}.{}.{} alpha", MAJOR, MINOR, PATCH);

/**
 * \class Common
 * \brief Класс содержит глобальные функции.
 */
class Common final
{
    using QSP = QStandardPaths;

public:
    Common() = delete;

    /**
     * \brief Инициализирует директорию для служебных файлов программы.
     *
     * \details Список возможных директорий, для хранения конфигурационных файл, предоставляет ОС.
     * Если в системе нет такого пути, который возвращает ОС, то создаём его вручную.
     *
     * > Для ОС Windows путь выглядит так: C:/Users/UserName/AppData/Local/OrganizationName/ApplicationName.
     *
     * > Для ОС Linux путь выглядит так: /home/UserName/.config/OrganizationName/ApplicationName
     *
     * > Для ОС macOS путь выглядит так: /Users/UserName/Library/Preferences/OrganizationName/ApplicationName
     */
    static void initialize()
    {
        // установка информации о программе
        qApp->setApplicationName(QString(ApplicationName.data()));
        qApp->setApplicationVersion(QString(ApplicatioVersion.data()));
        qApp->setOrganizationName(QString(OrganizationName.data()));
        qApp->setOrganizationDomain(QString(OrganizationDomain.data()));

        QDir dir;
        if (!dir.mkpath(settingsPath()))
            qWarning() << Q_FUNC_INFO << __LINE__ << ": Failed to create settings directory!";
    }

    /**
     * \brief Возвращает полное имя файла настроек.
     * \return полное имя файла настроек.
     */
    static QString settingsFile()
    {
        return settingsPath() + "/settings.json";
    }

    /**
     * \brief Возвращает полное имя файла настроек.
     * \return полное имя файла настроек.
     *
     * \details Операционная система возвращает список возможных мест расположения файлов настроек,
     * выбираем самый первый вариант и добавляем к нему название файла настроек.
     */
    static QString settingsPath()
    {        
        QStringList list(QSP::standardLocations(QSP::ConfigLocation));
#ifdef Q_OS_WIN
        return list.isEmpty() ? qApp->applicationDirPath() : list.first();
#else
        static constexpr std::string_view t_split("/");
        if (list.isEmpty())
            return qApp->applicationDirPath();
        return list.first() + t_split.data() + QString(OrganizationName.data()) + t_split.data() + QString(ApplicationName.data());
#endif
    }

private:
    Q_DISABLE_COPY(Common)
};

