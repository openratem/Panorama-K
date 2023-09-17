#pragma once

#include <QtCore>

class RigParser
{
public:
    RigParser() = default;
    explicit RigParser(const QString &fileName);
    explicit RigParser(QString &&fileName);

    void setFileName(const QString &fileName);

    QStringList groups();
    QStringList keys(const QString & group);
    QString value(const QString &group, const QString &key);

private:
    Q_DISABLE_COPY(RigParser)
    void clear();
    void load();
    void processLine(QByteArray &&array);

private:
    QString     m_fileName;
    QJsonObject m_container;

    QString     m_currentGroup;
    QJsonObject m_currentKeys;
};

