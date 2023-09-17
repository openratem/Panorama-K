#pragma once

#include "ui_Graphics.h"
#include <PowerSwrSeries.h>
#include <QTimer>
#include <QVector>

class Graphics final : public QWidget, private Ui::Graphics
{
    Q_OBJECT

    static constexpr qsizetype PointsIn1min { 1200 };
    static constexpr qsizetype PointsIn30min { 36000 };

public:
    explicit Graphics(PowerSwrSeries<PowerSwrData> *series, QWidget* parent = nullptr);

    float currentSWR() const;

public slots:
    void setAnalize(bool enable);
    void setFrequency(int freq);
    void setSwr(float swr);

    void clear();

private slots:
    void onUpdateGraphics();

private:
    void initialize();

private:
    PowerSwrSeries<PowerSwrData> *pPowerSwrSeries;
    QTimer m_timer;
    QMap<double, double> m_swrValues;
    QVector<PowerSwrData> m_readed;
    int m_currentFrequency {0};
    float m_currentSwr {1.0f};
};
