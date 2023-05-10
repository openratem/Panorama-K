#pragma once

#include <memory>
#include <QMainWindow>
#include <QJsonObject>
#include <PowerSwrSeries.h>
#include "../control/ControlObserver.h"
#include "Graphics/Graphics.h"
#include "ui_MainWindow.h"


class MainWindow final : public QMainWindow, protected Ui::MainWindow
{
    Q_OBJECT

public:
    /**
     * \brief Контсруктор по умолчанию.
     * \param parent - указатель на родительский класс.
     */
    explicit MainWindow(PowerSwrSeries<PowerSwrData> &series, QWidget *parent = nullptr);

    /**
     * \brief Установка настроек.
     * \param obj - настройки главного окна.
     */
    void setState(const QJsonObject &obj);

    /**
     * \brief Возвращает текущие настройки главного окна.
     */
    QJsonObject state() const;

    /**
     * \brief Возвращает экземпляр класса управления.
     */
    std::shared_ptr<ControlObserver> control();

private slots:
    void onProtocol(int index);
    void onOpen(bool state);
    void onConnectionStatus(PanoramaK::ConnectionStatus status);
    void onStatus(PanoramaK::Status status);
    void onTrxMode(bool trxMode);

private:
    void initialize();
    QJsonObject connectionSettings() const;

    QString description(PanoramaK::ConnectionStatus status) const;
    QString description(PanoramaK::Status status, bool details) const;
    QString description(bool trxMode) const;

private:
    PowerSwrSeries<PowerSwrData> &m_powerSwrSeries;
    Graphics *pGraphics {nullptr};
    std::shared_ptr<ControlObserver> pControl;
};

