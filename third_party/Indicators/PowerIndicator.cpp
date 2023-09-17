#include "PowerIndicator.h"

#include <QPainter>

PowerIndicator::PowerIndicator(QWidget* parent)
    : QWidget { parent }
{
    connect(this, SIGNAL(capacityChanged(quint16)), SLOT(repaint()));
}

void PowerIndicator::setCapacity(quint16 capacity)
{
    if (m_capacity != capacity) {
        m_capacity = capacity;
        emit capacityChanged(capacity);
    }
}

void PowerIndicator::paintEvent(QPaintEvent* event)
{
    QPainter painter_t(this);

    painter_t.setRenderHint(QPainter::Antialiasing);

    painter_t.setBrush(Qt::darkGray);

    double t_riskHeight = rect().height() - rect().height() * 0.3;
    double t_riskWidth = rect().width() * 0.01;
    double t_onePartWidth = (double)(rect().width()) / 150;

    auto t_firstRiskX = 0;
    auto t_secondRiskX = t_onePartWidth * 50 - t_riskWidth * 0.5;
    auto t_thirdRiskX = t_onePartWidth * 100 - t_riskWidth * 0.5;
    auto t_fourthRiskX = t_onePartWidth * 150 - t_riskWidth;

    painter_t.setFont(QFont("sans", 12, QFont::Bold));

    painter_t.drawText(t_firstRiskX + t_riskWidth * 0.2, rect().y() + 20, "0");
    painter_t.drawText(t_secondRiskX - t_riskWidth * 0.5, rect().y() + 20, "50");
    painter_t.drawText(t_thirdRiskX - t_riskWidth, rect().y() + 20, "100");
    painter_t.drawText(t_fourthRiskX - 30, rect().y() + 20, "150");

    painter_t.setPen(Qt::NoPen);

    painter_t.drawRect(QRectF(t_firstRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_secondRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_thirdRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_fourthRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));

    auto t_sectorHeight = (double)t_riskHeight * 0.7;

    painter_t.setBrush(Qt::red);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, rect().width(), t_sectorHeight);

    painter_t.setBrush(QColor { "#ffa500" });
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, t_onePartWidth * 125, t_sectorHeight);

    painter_t.setBrush(Qt::yellow);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, t_onePartWidth * 112.5, t_sectorHeight);

    painter_t.setBrush(QColor { "#008000" });
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, t_onePartWidth * 100, t_sectorHeight);

    painter_t.setBrush(Qt::darkGray);
    painter_t.drawRect(m_capacity * t_onePartWidth, rect().height() - t_sectorHeight + 1, rect().width(), t_sectorHeight);
}
