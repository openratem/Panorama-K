#pragma once

#include <mutex>
#include <QString>
#include <QJsonObject>


namespace ee {

/**
 * \class settings
 * \brief Запись и чтение настроек программы.
 */
class settings final
{
public:
    settings() = delete;
    explicit settings(const QString &path);

    /**
     * \brief Добавляет группу настроек.
     * \param key - название группы настроек.
     * \param value - настройки.
     */
    bool setValue(const QString &key, const QJsonObject &value);

    /**
     * \brief Возвращает настройки именованной группы.
     * \param key - название группы настроек.
     * \return настройки заданной группы.
     */
    [[nodiscard]] QJsonObject value(const QString &key) const;

    /**
     * \brief Записывает текущее состояние настроек в файл.
     */
    void save();

    /**
     * \brief Чтение настроек из файла.
     */
    void load();

    /**
     * \brief Записывает текущее состояние настроек в файл.
     */
    void exportTo(const QString &fileName);

    /**
     * \brief Чтение настроек из файла.
     */
    bool importFrom(const QString &fileName);

private:
    bool isCorrect() const;
    void backup();
    void recover();
    void makeMD5();

    QByteArray clacMD5(const QString &fileName) const;
    QByteArray loadMD5() const;

private:
    QString     m_path;
    QString     m_pathBack;
    QString     m_pathMd5;
    QJsonObject m_object;
    mutable std::mutex  m_mutex;
};

} // namespace ee


