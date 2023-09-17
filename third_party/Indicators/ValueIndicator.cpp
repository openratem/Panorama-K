#include "ValueIndicator.h"

#include <QPainter>

ValueIndicator::ValueIndicator(QWidget *parent)
    : QWidget{parent}
{
    connect(this, SIGNAL(valueChanged(double)), SLOT(repaint()));
    connect(this, SIGNAL(backgroundColorChanged(QColor)), SLOT(repaint()));
}

void ValueIndicator::setValue(double value)
{
    if (m_value != value) {
        m_value = value;
        emit valueChanged(value);
    }
}

void ValueIndicator::setPreffix(QString preffix)
{
    m_preffix = preffix;
}

void ValueIndicator::setSuffix(QString suffix)
{
    m_suffix = suffix;
}

void ValueIndicator::setBackgroundColor(QColor color)
{
    if (m_color != color) {
        m_color = color;
        emit backgroundColorChanged(color);
    }
}

void ValueIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter_t(this);

    painter_t.setRenderHint(QPainter::Antialiasing);

    painter_t.setFont(QFont("sans", 16, QFont::Bold));

    painter_t.setBrush(m_color);

    painter_t.drawRoundedRect(rect(), 5, 5);

    painter_t.setPen(Qt::black);

    painter_t.drawText(QRectF(10, rect().height()*0.5 - painter_t.fontMetrics().height()*0.5, rect().width(), rect().height()), m_preffix + QString::number(m_value) + m_suffix);
}
