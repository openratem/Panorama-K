#include "settings.h"

#include <stdexcept>

#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>

namespace ee {


settings::settings(QStringView path)
  : m_path(path.toString())
{
    //
    m_pathBack = path.toString() + QStringLiteral(".back");
    m_pathMd5  = path.toString() + QStringLiteral(".md5");

    // если файл отсутствует, то создаём его
    if (QFileInfo t_info(m_path); !t_info.exists()) {
        QFile t_file(m_path);
        t_file.open(QFile::WriteOnly);
        t_file.close();
    }

    // если файл отсутствует, то создаём его
    if (QFileInfo t_info(m_pathBack); !t_info.exists()) {
        QFile t_file(m_pathBack);
        t_file.open(QFile::WriteOnly);
        t_file.close();
    }

    // если файл отсутствует, то создаём его
    if (QFileInfo t_info(m_pathMd5); !t_info.exists()) {
        QFile t_file(m_pathMd5);
        t_file.open(QFile::WriteOnly);
        t_file.close();
    }
}

bool settings::setValue(QStringView key, const QJsonObject &value)
{
    std::lock_guard locker(m_mutex);

    if (!value.isEmpty() && !key.isEmpty()) {
        m_object[key] = value;
        return true;
    }
    return false;
}

QJsonObject settings::value(QStringView key) const
{
    std::lock_guard locker(m_mutex);

    if (!m_object.contains(key))
        return QJsonObject();
    return m_object[key].toObject();
}

void settings::save()
{
    std::lock_guard locker(m_mutex);

    // открываем файл для чтения
    QFile file(m_path);
    if (!file.open(QIODevice::WriteOnly))
        throw std::runtime_error("failed to open settings file for write");

    // запись всех данных в файл
    QJsonDocument document;
    document.setObject(m_object);
    file.write(document.toJson());

    // закрываем файл
    file.close();

    // записываем контрольную сумму MD5
    makeMD5();
}

void settings::load()
{
    std::lock_guard locker(m_mutex);

    // если файл *.json существует и существует *.json.md5
    if (isCorrect()) {
        // делаем резервную копию файла настроек
        backup();
    }
    else {
        // востанавливаю резервную копию, если она есть
        recover();
    }

    // открываем файл для чтения
    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error("failed to open settings file for read");

    // чтение всех данных файла
    QJsonDocument document(QJsonDocument::fromJson(file.readAll()));
    m_object = document.object();

    // закрываем файл
    file.close();
}

void settings::exportTo(QStringView fileName)
{
    std::lock_guard locker(m_mutex);

    // открываем файл для чтения
    QFile file(fileName.toString());
    if (!file.open(QIODevice::WriteOnly))
        return;

    // запись всех данных в файл
    QJsonDocument document;
    document.setObject(m_object);
    file.write(document.toJson());

    // закрываем файл
    file.close();
}

bool settings::importFrom(QStringView fileName)
{
    std::lock_guard locker(m_mutex);

    // открываем файл для чтения
    QFile file(fileName.toString());
    if (!file.open(QIODevice::ReadOnly))
        return false;

    // чтение всех данных файла
    QJsonDocument document(QJsonDocument::fromJson(file.readAll()));
    m_object = document.object();

    // закрываем файл
    file.close();

    return true;
}

bool settings::isCorrect() const
{
    auto t_md5Settings = clacMD5(m_path);
    auto t_md5Saved    = loadMD5();

    if (t_md5Saved.isEmpty() && t_md5Settings.isEmpty())
        return false;

    if (t_md5Saved.isEmpty() && !t_md5Settings.isEmpty())
        return true;

    return t_md5Saved == t_md5Settings;
}

void settings::backup()
{
    // удаляем резервный файл
    QDir t_dir;
    t_dir.remove(m_pathBack);

    // создаём резервный файл
    QFile::copy(m_path, m_pathBack);
}

void settings::recover()
{
    auto t_md5Settings = clacMD5(m_pathBack);
    auto t_md5Saved    = loadMD5();

    if (t_md5Settings.isEmpty() || t_md5Saved.isEmpty())
        return;

    if (t_md5Settings == t_md5Saved) {
        // удаляем файл
        QDir t_dir;
        t_dir.remove(m_path);

        // фосстанавливаем резервный файл
        QFile::copy(m_pathBack, m_path);
    }
}

void settings::makeMD5()
{
    if (QFile t_file(m_pathMd5); t_file.open(QFile::WriteOnly)) {
        t_file.write(clacMD5(m_path));
        t_file.close();
    }
}

QByteArray settings::loadMD5() const
{
    if (QFile t_file(m_pathMd5); t_file.open(QFile::ReadOnly))
        return t_file.readAll();
    return QByteArray();
}

QByteArray settings::clacMD5(QStringView fileName) const
{
    if (QFile t_file(fileName.toString()); t_file.open(QFile::ReadOnly)) {
        QCryptographicHash t_hash(QCryptographicHash::Md5);
        t_hash.addData(t_file.readAll());
        t_file.close();
        return t_hash.result();
    }
    return QByteArray();
}

}
