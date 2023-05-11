#pragma once

#include <QVector>
#include <QTimer>
#include <PowerSwrSeries.h>
#include "ui_Graphics.h"

class Graphics final : public QWidget, private Ui::Graphics
{
    Q_OBJECT

    static constexpr qsizetype PointsIn1min {1200};
    static constexpr qsizetype PointsIn30min {36000};

public:
    explicit Graphics(PowerSwrSeries<PowerSwrData> &series, QWidget *parent = nullptr);

    float currentSWR() const;

public slots:
    void setAnalize(bool enable);

private slots:
    void onUpdateGraphics();

private:
    void initialize();

private:
    PowerSwrSeries<PowerSwrData> &m_powerSwrSeries;
    QTimer m_timer;

    QVector<PowerSwrData> m_readed;

    QVector<double> m_timeSeries1min;
    QVector<double> m_powerSeries1min;
    QVector<double> m_swrSeries1min;

    QVector<double> m_timeSeries30min;
    QVector<double> m_powerSeries30min;
    QVector<double> m_swrSeries30min;
};


