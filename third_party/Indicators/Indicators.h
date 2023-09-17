#pragma once

#include <QWidget>

class Indicators : public QWidget
{
    Q_OBJECT

public:
    explicit Indicators(QWidget *parent = nullptr);

    void setCapacity(quint16 capacity);

signals:
    void capacityChanged(quint16);

private:
    void paintEvent(QPaintEvent *event) override;

private:
    quint16 m_capacity {0};
};
