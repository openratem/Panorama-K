#include "Indicators.h"

#include <QPainter>

Indicators::Indicators(QWidget *parent)
    : QWidget{parent}
{
    connect(this, SIGNAL(capacityChanged(quint16)), SLOT(repaint()));
}

void Indicators::setCapacity(quint16 capacity)
{
    if (m_capacity != capacity) {
        m_capacity = capacity;
        emit capacityChanged(capacity);
    }
}

void Indicators::paintEvent(QPaintEvent *event)
{
    QPainter painter_t(this);

    painter_t.setRenderHint(QPainter::Antialiasing);

    painter_t.setBrush(Qt::darkGray);

    double t_riskHeight = rect().height() - rect().height() * 0.3;
    double t_riskWidth = rect().width() * 0.01;
    double t_onePartWidth = (double)(rect().width()) / 200;

    qDebug() << rect().width() << t_onePartWidth << t_onePartWidth * 200;

    auto t_firstRiskX = 0;
    auto t_secondRiskX = t_onePartWidth * 75 - t_riskWidth * 0.5;
    auto t_thirdRiskX = t_onePartWidth * 125 - t_riskWidth * 0.5;
    auto t_fourthRiskX = t_onePartWidth * 200 - t_riskWidth;

    painter_t.drawText(t_firstRiskX  + t_riskWidth * 0.2, rect().y() + 10, "0");
    painter_t.drawText(t_secondRiskX - t_riskWidth * 0.5, rect().y() + 10, "75");
    painter_t.drawText(t_thirdRiskX  - t_riskWidth,       rect().y() + 10, "125");
    painter_t.drawText(t_fourthRiskX - t_riskWidth * 1.7, rect().y() + 10, "200");

    painter_t.setPen(Qt::NoPen);

    painter_t.drawRect(QRectF(t_firstRiskX,  rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_secondRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_thirdRiskX,  rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_fourthRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));

    auto t_sectorHeight = (double)t_riskHeight * 0.7;

    painter_t.setBrush(Qt::red);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, rect().width(), t_sectorHeight);

    painter_t.setBrush(QColor{"#ffa500"});
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, t_onePartWidth * 125, t_sectorHeight);

    painter_t.setBrush(Qt::yellow);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, t_onePartWidth * 100, t_sectorHeight);

    painter_t.setBrush(Qt::green);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, t_onePartWidth * 75, t_sectorHeight);

    painter_t.setBrush(Qt::darkGray);
    painter_t.drawRect(m_capacity * t_onePartWidth, rect().height() - t_sectorHeight + 1, rect().width(), t_sectorHeight);
}
