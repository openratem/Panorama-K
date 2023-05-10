#include "Graphics.h"
#include <chrono>

using namespace std::chrono_literals;

Graphics::Graphics(PowerSwrSeries<PowerSwrData> &series, QWidget *parent)
  : QWidget{parent}, m_powerSwrSeries{series}
{
    setupUi(this);

    initialize();

    connect(&m_timer, &QTimer::timeout, this, &Graphics::onUpdateGraphics);
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
    // чтение данных
    if (m_readed.resize(m_powerSwrSeries.readCount()); !m_powerSwrSeries.readAll(m_readed))
        return;

    m_timeSeries1min.clear();
    m_powerSeries1min.clear();
    m_swrSeries1min.clear();

    m_timeSeries30min.clear();
    m_powerSeries30min.clear();
    m_swrSeries30min.clear();

    // подготовка данных для графика мощности и КСВ на 1 минуту
    double t_x {0}, t_step {0.05};
    for (auto t_begin = m_readed.rbegin(); t_begin != (m_readed.rbegin() + qMin<int>(PointsIn1min, m_readed.size())); t_begin++) {
        m_timeSeries1min.push_back(t_x);
        m_powerSeries1min.push_back((*t_begin).power);
        m_swrSeries1min.push_back((*t_begin).swr);
        t_x += t_step;
    }

    // подготовка данных для графика мощности и КСВ на 30 минуту
    t_x = 0, t_step = 0.05/60.0;
    for (auto t_begin = m_readed.rbegin(); t_begin != (m_readed.rbegin() + qMin<int>(PointsIn30min, m_readed.size())); t_begin++) {
        m_timeSeries30min.push_back(t_x);
        m_powerSeries30min.push_back((*t_begin).power);
        m_swrSeries30min.push_back((*t_begin).swr);
        t_x += t_step;
    }

    //
    pPlotter1min->graph(0)->setData(m_timeSeries1min, m_swrSeries1min);
    pPlotter1min->graph(1)->setData(m_timeSeries1min, m_powerSeries1min);
    pPlotter1min->replot();

    pPlotter30min->graph(0)->setData(m_timeSeries30min, m_swrSeries30min);
    pPlotter30min->graph(1)->setData(m_timeSeries30min, m_powerSeries30min);
    pPlotter30min->replot();
}

void Graphics::initialize()
{
    //
    m_timeSeries1min.reserve(PointsIn1min);
    m_powerSeries1min.reserve(PointsIn1min);
    m_swrSeries1min.reserve(PointsIn1min);

    m_timeSeries30min.reserve(PointsIn30min);
    m_powerSeries30min.reserve(PointsIn30min);
    m_swrSeries30min.reserve(PointsIn30min);

    // инициализация графика наблюдения в течение 1 минуты
    pPlotter1min->setLocale(QLocale(QLocale::Russian, QLocale::Russia));
    pPlotter1min->legend->setVisible(true);
    // установка заголовков для шкал
    pPlotter1min->yAxis2->setVisible(true);
    pPlotter1min->xAxis->setLabel("Время, сек");
    pPlotter1min->yAxis->setLabel("КСВ");
    pPlotter1min->yAxis2->setLabel("Мощность, Вт");
    // установка границ графика
    pPlotter1min->xAxis->setRange(0, 60);
    pPlotter1min->yAxis->setRange(1, 11);
    pPlotter1min->xAxis2->setRange(0, 60);
    pPlotter1min->yAxis2->setRange(0, 120);
    // добавление заголовка
    pPlotter1min->plotLayout()->insertRow(0);
    pPlotter1min->plotLayout()->addElement(0, 0, new QCPTextElement(pPlotter1min, "График мощности и КСВ", QFont("sans", 14, QFont::Bold)));
    // добавление графика КСВ
    pPlotter1min->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(46, 125, 50));
        pen.setWidth(2);
        pPlotter1min->graph(0)->setPen(pen);
    }
    pPlotter1min->graph(0)->setLineStyle(QCPGraph::lsLine);
    pPlotter1min->graph(0)->setName("КСВ");
    // добавление графика мощности
    pPlotter1min->addGraph(pPlotter1min->xAxis2, pPlotter1min->yAxis2);
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter1min->graph(1)->setPen(pen);
    }
    pPlotter1min->graph(1)->setLineStyle(QCPGraph::lsLine);
    pPlotter1min->graph(1)->setName("Мощность");

    // инициализация графика наблюдения в течение 30 минут
    pPlotter30min->setLocale(QLocale(QLocale::Russian, QLocale::Russia));
    pPlotter30min->legend->setVisible(true);
    // установка заголовков для шкал
    pPlotter30min->yAxis2->setVisible(true);
    pPlotter30min->xAxis->setLabel("Время, мин");
    pPlotter30min->yAxis->setLabel("КСВ");
    pPlotter30min->yAxis2->setLabel("Мощность, Вт");
    // установка границ графика
    pPlotter30min->xAxis->setRange(0, 30);
    pPlotter30min->yAxis->setRange(1, 11);
    pPlotter30min->xAxis2->setRange(0, 30);
    pPlotter30min->yAxis2->setRange(0, 120);
    // добавление заголовка
    pPlotter30min->plotLayout()->insertRow(0);
    pPlotter30min->plotLayout()->addElement(0, 0, new QCPTextElement(pPlotter30min, "График мощности и КСВ", QFont("sans", 14, QFont::Bold)));
    // добавление графика КСВ
    pPlotter30min->addGraph();
    {
        QPen pen;
        pen.setColor(QColor(46, 125, 50));
        pen.setWidth(2);
        pPlotter30min->graph(0)->setPen(pen);
    }
    pPlotter30min->graph(0)->setLineStyle(QCPGraph::lsLine);
    pPlotter30min->graph(0)->setName("КСВ");
    // добавление графика мощности
    pPlotter30min->addGraph(pPlotter30min->xAxis2, pPlotter30min->yAxis2);
    {
        QPen pen;
        pen.setColor(QColor(190, 55, 12));
        pen.setWidth(2);
        pPlotter30min->graph(1)->setPen(pen);
    }
    pPlotter30min->graph(1)->setLineStyle(QCPGraph::lsLine);
    pPlotter30min->graph(1)->setName("Мощность");
}
