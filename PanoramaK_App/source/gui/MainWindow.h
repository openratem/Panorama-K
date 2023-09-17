#pragma once

#include "../control/ControlObserver.h"
#include "Graphics/Graphics.h"
#include "ui_MainWindow.h"
#include <PowerSwrSeries.h>
#include <QJsonObject>
#include <QMainWindow>
#include <memory>
#ifdef PANORAMAK_LIB_LOADER
#  include "../service/lib/ServiceLib.h"
#endif

#ifdef PANORAMAK_LIB_LOADER
using ControlName = ControlType;
#else
using ControlName = PanoramaK::ControlInterface;
#endif

class MainWindow final : public QMainWindow, protected Ui::MainWindow
{
    Q_OBJECT

public:
    /**
     * \brief Контсруктор по умолчанию.
     * \param parent - указатель на родительский класс.
     */
#ifdef PANORAMAK_LIB_LOADER
    explicit MainWindow(PowerSwrSeries<PowerSwrData>* series, DeviceInfo* newDevice, QWidget* parent = nullptr);
#else
    explicit MainWindow(PowerSwrSeries<PowerSwrData>* series, QWidget* parent = nullptr);
#endif

    /**
     * \brief Установка настроек.
     * \param obj - настройки главного окна.
     */
    void setState(const QJsonObject& obj);

    /**
     * \brief Возвращает текущие настройки главного окна.
     */
    QJsonObject state() const;

    /**
     * \brief Возвращает экземпляр класса управления.
     */
    std::shared_ptr<ControlObserver> control();

public slots:
    void setSwr(float value);
    void setWatts(float watts);
    void onError(const QString &errorString);

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
    PowerSwrSeries<PowerSwrData> *pPowerSwrSeries;
    Graphics *pGraphics{nullptr};
    std::shared_ptr<ControlObserver> pControl;
    float m_swr {1.0f};
    float m_watts {0.0f};
#ifdef PANORAMAK_LIB_LOADER
    DeviceInfo *pDeviceInfo;
#endif
};
