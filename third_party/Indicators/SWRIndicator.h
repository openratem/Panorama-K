#pragma once

#include <QWidget>

class SWRIndicator : public QWidget {
    Q_OBJECT
public:
    explicit SWRIndicator(QWidget* parent = nullptr);

    void setSWR(double swr);

signals:
    void swrChanged(double);

private:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_swr { 1 };
};
