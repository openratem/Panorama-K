#include "Graphics.h"
#include <chrono>

using namespace std::chrono_literals;

Graphics::Graphics(PowerSwrSeries<PowerSwrData>* series, QWidget* parent)
    : QWidget { parent }
    , pPowerSwrSeries { series }
{
    setupUi(this);

    initialize();

    connect(pClearButton, &QPushButton::clicked, this, &Graphics::clear);
    connect(&m_timer, &QTimer::timeout, this, &Graphics::onUpdateGraphics);
}

void Graphics::setFrequency(int freq)
{
    m_currentFrequency = freq;

    if (!m_swrValues.contains(freq))
        m_swrValues.insert(freq, 1);
}

void Graphics::setSwr(float swr)
{
    m_currentSwr = swr;

    if (m_swrValues.contains(m_currentFrequency))
        m_swrValues[m_currentFrequency] = swr;
}

float Graphics::currentSWR() const
{
    return m_currentSwr;
}

void Graphics::setAnalize(bool enable)
{
    if (enable)
        m_timer.start(50ms);
    else
        m_timer.stop();
}

void Graphics::onUpdateGraphics()
{
    if (m_swrValues.isEmpty())
        return;

    pPlotter->graph(0)->data().clear();
    pPlotter->graph(1)->data().clear();
    pPlotter->graph(2)->data().clear();
    pPlotter->graph(3)->data().clear();

    if (m_swrValues.contains(m_currentFrequency)) {
        if (m_swrValues.value(m_currentFrequency) < 1.5) {
            pPlotter->graph(0)->setPen(QPen(QBrush("#008000"), 2));
            pPlotter->graph(0)->addData(double(m_currentFrequency) / 1000000, m_swrValues.value(m_currentFrequency));
        } else if (m_swrValues.value(m_currentFrequency) < 2) {
            pPlotter->graph(1)->setPen(QPen(QBrush(Qt::yellow), 2));
            pPlotter->graph(1)->addData(double(m_currentFrequency) / 1000000, m_swrValues.value(m_currentFrequency));
        } else if (m_swrValues.value(m_currentFrequency) < 3) {
            pPlotter->graph(2)->setPen(QPen(QBrush("#ffa500"), 2));
            pPlotter->graph(2)->addData(double(m_currentFrequency) / 1000000, m_swrValues.value(m_currentFrequency));
        } else {
            pPlotter->graph(3)->setPen(QPen(QBrush(Qt::red), 2));
            pPlotter->graph(3)->addData(double(m_currentFrequency) / 1000000, m_swrValues.value(m_currentFrequency));
        }
    }

    pPlotter->replot();
}

void Graphics::clear()
{
    pPlotter->clearGraphs();
    m_swrValues.clear();

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(46, 125, 50));
        pen.setWidth(2);
        pPlotter->graph(0)->setPen(pen);
    }

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter->graph(1)->setPen(pen);
    }

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter->graph(2)->setPen(pen);
    }

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter->graph(3)->setPen(pen);
    }

    pPlotter->graph(0)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(0)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->graph(1)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(1)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->graph(2)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(2)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->graph(3)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(3)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->replot();
}

void Graphics::initialize()
{
    // инициализация графика наблюдения в течение 1 минуты
    pPlotter->setLocale(QLocale(QLocale::Russian, QLocale::Russia));
    pPlotter->legend->setVisible(false);
    // установка заголовков для шкал
    pPlotter->xAxis->setLabel("МГц");
    pPlotter->yAxis->setLabel("КСВ");
    // установка границ графика
    pPlotter->yAxis->setRange(0.9, 11.1);
    pPlotter->xAxis->setRange(1.5, 30);
    // добавление заголовка
    pPlotter->plotLayout()->insertRow(0);
    pPlotter->plotLayout()->addElement(0, 0, new QCPTextElement(pPlotter, "График КСВ", QFont("sans", 14, QFont::Bold)));
    // добавление графика КСВ
    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(46, 125, 50));
        pen.setWidth(2);
        pPlotter->graph(0)->setPen(pen);
    }

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter->graph(1)->setPen(pen);
    }

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter->graph(1)->setPen(pen);
    }

    pPlotter->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter->graph(1)->setPen(pen);
    }

    pPlotter->graph(0)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(0)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->graph(1)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(1)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->graph(2)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(2)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    pPlotter->graph(3)->setLineStyle(QCPGraph::lsNone);
    pPlotter->graph(3)->setScatterStyle(QCPScatterStyle::ssCrossCircle);
}
