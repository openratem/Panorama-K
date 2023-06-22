#include "RigParser.h"

RigParser::RigParser(const QString &fileName)
  : m_fileName{fileName}
{
    load();
}

RigParser::RigParser(QString &&fileName)
  : m_fileName {std::move(fileName)}
{
    load();
}

void RigParser::setFileName(const QString &fileName)
{
    clear();

    if (m_fileName == fileName)
        return;

    if (!m_fileName.isEmpty())
        clear();

    m_fileName = fileName;
    load();
}

QStringList RigParser::groups()
{
    return m_container.keys();
}

QStringList RigParser::keys(const QString &group)
{
    if (!m_container.contains(group))
        return QStringList();

    return m_container[group].toObject().keys();
}

QString RigParser::value(const QString &group, const QString &key)
{
    if (!m_container.contains(group))
        return QString();

    if (!m_container[group].toObject().contains(key))
        return QString();

    return m_container[group].toObject()[key].toString();
}

void RigParser::clear()
{
    m_fileName.clear();
    m_container = QJsonObject();

    m_currentGroup.clear();
    m_currentKeys = QJsonObject();
}

void RigParser::load()
{
    // открываем файл для чтения
    QFile t_file(m_fileName);
    if (!t_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    // читаем построчно, что бы отбросить комментарии
    while (!t_file.atEnd())
        processLine(t_file.readLine());

    // обрабатываем последнюю группу
    if (!m_currentGroup.isEmpty())
        m_container[m_currentGroup] = m_currentKeys;

    // закрываем файл
    t_file.close();
}

void RigParser::processLine(QByteArray &&array)
{
    QRegularExpression t_regExp("[\n\r]+");
    QString t_text = QString::fromLatin1(array).remove(t_regExp);

    if (t_text.isEmpty())
        return;

    if (array.at(0) == ';')
        return;

    // определение группы
    if (t_text.contains("[") && t_text.contains("]")) {
        // сохранение предыдущей группы
        if (!m_currentGroup.isEmpty())
            m_container[m_currentGroup] = m_currentKeys;

        // начало группы
        m_currentGroup = t_text.remove("[").remove("]");
        m_currentKeys  = QJsonObject();
        return;
    }

    // определение полей
    if (t_text.contains("=")) {
        QStringList t_list = t_text.split("=", Qt::SkipEmptyParts);
        if (t_list.count() != 2)
            return;
        m_currentKeys[t_list.first()] = t_list.last();
    }
}


























