#pragma once

#include <QObject>

class SystemTray final : public QObject
{
    Q_OBJECT

public:
    explicit SystemTray(QObject *parent = nullptr);

signals:

};

