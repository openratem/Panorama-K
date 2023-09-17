#include "SWRIndicator.h"

#include <QPainter>

SWRIndicator::SWRIndicator(QWidget* parent)
    : QWidget { parent }
{
    connect(this, SIGNAL(swrChanged(double)), SLOT(repaint()));
}

void SWRIndicator::setSWR(double swr)
{
    if (m_swr != swr) {
        m_swr = swr;
        emit swrChanged(swr);
    }
}

void SWRIndicator::paintEvent(QPaintEvent* event)
{
    QPainter painter_t(this);

    painter_t.setRenderHint(QPainter::Antialiasing);

    painter_t.setBrush(Qt::darkGray);

    auto t_riskHeight = rect().height() - rect().height() * 0.3;
    auto t_riskWidth = rect().width() * 0.01;

    auto t_firstRiskX = 0;
    auto t_secondRiskX = rect().width() * std::log10(1.5);
    auto t_thirdRiskX = rect().width() * std::log10(2);
    auto t_fourthRiskX = rect().width() * std::log10(3);
    auto t_fifthRiskX = rect().width() * std::log10(10);

    painter_t.setFont(QFont("sans", 12, QFont::Bold));

    painter_t.drawText(t_firstRiskX + t_riskWidth * 0.2, rect().y() + 20, "1.0");
    painter_t.drawText(t_secondRiskX - t_riskWidth * 0.5, rect().y() + 20, "1.5");
    painter_t.drawText(t_thirdRiskX - t_riskWidth, rect().y() + 20, "2.0");
    painter_t.drawText(t_fourthRiskX - t_riskWidth * 0.7, rect().y() + 20, "3.0");
    painter_t.drawText(t_fifthRiskX - 25, rect().y() + 20, "10");

    painter_t.setPen(Qt::NoPen);

    painter_t.drawRect(QRectF(t_firstRiskX, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_secondRiskX - t_riskWidth * 0.5, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_thirdRiskX - t_riskWidth * 0.5, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_fourthRiskX - t_riskWidth * 0.5, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));
    painter_t.drawRect(QRectF(t_fifthRiskX - t_riskWidth, rect().height() - t_riskHeight, t_riskWidth, t_riskHeight));

    auto t_sectorHeight = (double)t_riskHeight * 0.7;

    painter_t.setBrush(Qt::red);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, rect().width(), t_sectorHeight);

    painter_t.setBrush(QColor { "#ffa500" });
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, rect().width() * std::log10(3), t_sectorHeight);

    painter_t.setBrush(Qt::yellow);
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, rect().width() * std::log10(2), t_sectorHeight);

    painter_t.setBrush(QColor { "#008000" });
    painter_t.drawRect(0, rect().height() - t_sectorHeight + 1, rect().width() * std::log10(1.5), t_sectorHeight);

    painter_t.setBrush(Qt::darkGray);
    painter_t.drawRect(double(rect().width()) * std::log10(m_swr), rect().height() - t_sectorHeight + 1, rect().width(), t_sectorHeight);
}
