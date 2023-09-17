#include "MainWindow.h"
#include <QMessageBox>
#include <QtSerialPort>
#include <memory>

#ifdef PANORAMAK_LIB_LOADER
MainWindow::MainWindow(PowerSwrSeries<PowerSwrData>* series, DeviceInfo *newDevice, QWidget* parent)
  : QMainWindow{parent},
    pDeviceInfo{newDevice},
    pPowerSwrSeries{series}
#else
MainWindow::MainWindow(PowerSwrSeries<PowerSwrData>* series, QWidget* parent)
  : QMainWindow{parent},
    pPowerSwrSeries{series}
#endif
{
    setupUi(this);
    initialize();
}

void MainWindow::initialize()
{
    pControl = std::make_shared<ControlObserver>();

#ifdef __linux__
    this->setStyleSheet("background-color: #202020");
#endif

    //
    pGraphics = std::make_unique<Graphics>(pPowerSwrSeries).release();
    pLayoutService->addWidget(pGraphics);

    // загрузка списка поддерживаемых трансиверов
#ifndef PANORAMAK_LIB_LOADER
    for (const auto& protocol : PanoramaK::TransceiverFactory::devices())
        cbTransceivers->addItem(protocol.name, static_cast<int>(protocol.controlInterface));
#else
    for (auto pInfo = pDeviceInfo; pInfo->next != nullptr;) {
        cbTransceivers->addItem(pInfo->name, static_cast<int>(pInfo->controlType));
        pInfo = pInfo->next;
    }
#endif

    // получение доступных COM портов
    for (const auto& info : QSerialPortInfo::availablePorts()) {
        cbComportName->addItem(info.portName());
        cbMeterComportName->addItem(info.portName());
    }

    // настройка системы событий
    connect(pControl.get(), &ControlObserver::protocolChanged, cbTransceivers, &QComboBox::setCurrentIndex);
    connect(pControl.get(), &ControlObserver::connectionStatusChanged, this, &MainWindow::onConnectionStatus);
    connect(pControl.get(), &ControlObserver::trxChanged, pGraphics, &Graphics::setAnalize);
    connect(pControl.get(), &ControlObserver::trxChanged, this, &MainWindow::onTrxMode);
    connect(pControl.get(), &ControlObserver::statusChanged, this, &MainWindow::onStatus);
    connect(pControl.get(), &ControlObserver::frequencyChanged, pGraphics, &Graphics::setFrequency);
    connect(pControl.get(), &ControlObserver::frequencyChanged, lbFrequency, [&](auto Hz) { lbFrequency->setText(QString::number(static_cast<double>(Hz) / 1000) + " КГц"); });
    connect(cbTransceivers, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onProtocol);
    connect(pbNetworkConnect, &QPushButton::toggled, this, &MainWindow::onOpen);
    connect(pbSerialPortOpen, &QPushButton::toggled, this, &MainWindow::onOpen);
    connect(swrIndicatorWidget, &SWRIndicator::swrChanged, swrValueIndicatorWidget, &ValueIndicator::setValue);
    connect(powerIndicatorWidget, &PowerIndicator::capacityChanged, powerValueIndicatorWidget, &ValueIndicator::setValue);

    // начальная инициализация
    tabWidget->setCurrentIndex(0);

    powerValueIndicatorWidget->setSuffix(" Вт");
    swrValueIndicatorWidget->setPreffix("КСВ: ");

    onProtocol(cbTransceivers->currentIndex());
    onStatus(pControl->status());
    onConnectionStatus(pControl->connectionStatus());
    onTrxMode(pControl->trx());
}

void MainWindow::setState(const QJsonObject& obj)
{
    if (obj.contains("protocol")) {
        if (const auto t_protocol = obj["protocol"].toString(); !t_protocol.isEmpty()) {
            if (const auto t_index = cbTransceivers->findText(t_protocol); t_index >= 0)
                cbTransceivers->setCurrentIndex(t_index);
        }
    }

    if (obj.contains("networkAddress"))
        leHostAddress->setText(obj["networkAddress"].toString());

    if (obj.contains("networkOpen"))
        pbNetworkConnect->setChecked(obj["networkOpen"].toBool());

    if (obj.contains("comPortAnalizer")) {
        if (const auto t_portName = obj["comPort"].toString(); !t_portName.isEmpty()) {
            if (const auto t_index = cbMeterComportName->findText(t_portName); t_index >= 0)
                cbMeterComportName->setCurrentIndex(t_index);
        }
    }

    if (obj.contains("comPort")) {
        if (const auto t_portName = obj["comPort"].toString(); !t_portName.isEmpty()) {
            if (const auto t_index = cbComportName->findText(t_portName); t_index >= 0)
                cbComportName->setCurrentIndex(t_index);
        }
    }

    if (obj.contains("comParity"))
        cbComportParity->setCurrentIndex(obj["comParity"].toInt());

    if (obj.contains("comData"))
        cbComportData->setCurrentIndex(obj["comData"].toInt());

    if (obj.contains("comBaudrate"))
        cbComportBaudrate->setCurrentIndex(obj["comBaudrate"].toInt());

    if (obj.contains("comDtr"))
        cbComportDtr->setCurrentIndex(obj["comDtr"].toInt());

    if (obj.contains("comRts"))
        cbComportRts->setCurrentIndex(obj["comRts"].toInt());

    if (obj.contains("comOpen"))
        pbSerialPortOpen->setChecked(obj["comOpen"].toBool());
}

QJsonObject MainWindow::state() const
{
    QJsonObject t_obj;

    t_obj["protocol"] = cbTransceivers->currentText();

    t_obj["networkAddress"] = leHostAddress->text();
    t_obj["networkOpen"] = QJsonValue(pbNetworkConnect->isChecked());

    t_obj["comPortAnalizer"] = cbMeterComportName->currentText();
    t_obj["comPort"] = cbComportName->currentText();
    t_obj["comParity"] = cbComportParity->currentIndex();
    t_obj["comData"] = cbComportData->currentIndex();
    t_obj["comBaudrate"] = cbComportBaudrate->currentIndex();
    t_obj["comDtr"] = cbComportDtr->currentIndex();
    t_obj["comRts"] = cbComportRts->currentIndex();
    t_obj["comOpen"] = QJsonValue(pbSerialPortOpen->isChecked());

    return t_obj;
}

std::shared_ptr<ControlObserver> MainWindow::control()
{
    return pControl;
}

void MainWindow::setSwr(float value)
{
    m_swr = value;

    swrIndicatorWidget->setSWR(value);
    pGraphics->setSwr(value);

    if (value < 1.5) {
        swrValueIndicatorWidget->setBackgroundColor("#008000");
    } else if (value >= 1.5 && value < 2) {
        swrValueIndicatorWidget->setBackgroundColor(Qt::yellow);
    } else if (value >= 2 && value < 3) {
        swrValueIndicatorWidget->setBackgroundColor("#ffa500");
    } else if (value >= 3) {
        swrValueIndicatorWidget->setBackgroundColor(Qt::red);
    }
}

void MainWindow::setWatts(float watts)
{
    m_watts = watts;

    powerIndicatorWidget->setCapacity(watts);

    if (watts < 100) {
        powerValueIndicatorWidget->setBackgroundColor("#008000");
    } else if (watts >= 100 && watts < 112.5) {
        powerValueIndicatorWidget->setBackgroundColor(Qt::yellow);
    } else if (watts >= 112.5 && watts < 125) {
        powerValueIndicatorWidget->setBackgroundColor("#ffa500");
    } else if (watts >= 125) {
        powerValueIndicatorWidget->setBackgroundColor(Qt::red);
    }
}

void MainWindow::onError(const QString& errorString)
{
    QMessageBox::warning(this, "Error", errorString);

    bool state = false;
    // управление блокировкой элементов управления
    pbNetworkConnect->setChecked(state);
    pbSerialPortOpen->setChecked(state);
    //    lbProtocol->setDisabled(state);
    //    lbPortName->setDisabled(state);
    //    cbComportName->setDisabled(state);
    //    lbParity->setDisabled(state);
    //    cbComportParity->setDisabled(state);
    //    lbBits->setDisabled(state);
    //    cbComportData->setDisabled(state);
    //    lbBaudrate->setDisabled(state);
    //    cbComportBaudrate->setDisabled(state);
    //    lbDtr->setDisabled(state);
    //    cbComportDtr->setDisabled(state);
    //    lbRts->setDisabled(state);
    //    cbComportRts->setDisabled(state);
    //    lbHostAddress->setDisabled(state);
    //    leHostAddress->setDisabled(state);
    //    cbTransceivers->setDisabled(state);
    //    cbMeterComportName->setDisabled(state);
}

void MainWindow::onProtocol(int index)
{
    swConnectConfiguration->setCurrentIndex(cbTransceivers->itemData(index).toInt());
    pControl->setProtocol(index);
}

void MainWindow::onOpen(bool state)
{
    // управление блокировкой элементов управления
    lbProtocol->setDisabled(state);
    lbPortName->setDisabled(state);
    cbComportName->setDisabled(state);
    lbParity->setDisabled(state);
    cbComportParity->setDisabled(state);
    lbBits->setDisabled(state);
    cbComportData->setDisabled(state);
    lbBaudrate->setDisabled(state);
    cbComportBaudrate->setDisabled(state);
    lbDtr->setDisabled(state);
    cbComportDtr->setDisabled(state);
    lbRts->setDisabled(state);
    cbComportRts->setDisabled(state);
    lbHostAddress->setDisabled(state);
    leHostAddress->setDisabled(state);
    cbTransceivers->setDisabled(state);
    cbMeterComportName->setDisabled(state);

    //
    if (state)
        pControl->open(connectionSettings(), state);
    else
        pControl->close(state);
}

QJsonObject MainWindow::connectionSettings() const
{
    QJsonObject t_config;
    const auto t_interface = static_cast<ControlName>(cbTransceivers->currentData().toInt());

    if (t_interface == ControlName::Network) {
        QJsonObject t_network;
        t_network["address"] = leHostAddress->text();

        t_config["network"] = t_network;
    } else if (t_interface == ControlName::SerialPort) {
        QJsonObject t_serialPort;
        t_serialPort["name"] = cbComportName->currentText();
        t_serialPort["parity"] = cbComportParity->currentText();
        t_serialPort["data"] = cbComportData->currentText().toInt();
        t_serialPort["baudrate"] = cbComportBaudrate->currentText().toInt();
        t_serialPort["dtr"] = cbComportDtr->currentIndex() == 1 ? 1 : 0;
        t_serialPort["rts"] = cbComportRts->currentIndex() == 1 ? 1 : 0;

        t_config["serialPort"] = t_serialPort;

        QJsonObject t_serialPortAnalizer;
        t_serialPortAnalizer["name"] = cbMeterComportName->currentText();
        t_serialPortAnalizer["parity"] = "Нет";
        t_serialPortAnalizer["data"] = 8;
        t_serialPortAnalizer["baudrate"] = 9600;
        t_serialPortAnalizer["dtr"] = 0;
        t_serialPortAnalizer["rts"] = 0;

        t_config["analizer"] = t_serialPortAnalizer;
    }

    return t_config;
}

void MainWindow::onConnectionStatus(PanoramaK::ConnectionStatus status)
{
    if (status == PanoramaK::ConnectionStatus::Connecting) {
        QString t_title = tr("Подключение");
        pbSerialPortOpen->setText(t_title);
        pbNetworkConnect->setText(t_title);
    } else if (status == PanoramaK::ConnectionStatus::Connected) {
        QString t_title = tr("Подключено");
        pbSerialPortOpen->setText(t_title);
        pbNetworkConnect->setText(t_title);
    } else {
        QString t_title = tr("Подключиться");
        pbSerialPortOpen->setText(t_title);
        pbNetworkConnect->setText(t_title);
    }

    lbTrxMode->setVisible(status == PanoramaK::ConnectionStatus::Connected);
    lbAtuStatus->setVisible(status == PanoramaK::ConnectionStatus::Connected);
    lbAtuDescription->setVisible(status == PanoramaK::ConnectionStatus::Connected);

    lbConnectStatus->setText(description(status));
}

void MainWindow::onStatus(PanoramaK::Status status)
{
    lbAtuStatus->setText(description(status, false));
    lbAtuDescription->setText(description(status, true));
}

void MainWindow::onTrxMode(bool trxMode)
{
    lbTrxMode->setText(description(trxMode));
}

QString MainWindow::description(PanoramaK::ConnectionStatus status) const
{
    switch (status) {
    case PanoramaK::ConnectionStatus::Disconnect:
        return tr("Выполните подключение к трансиверу").toUpper();
    case PanoramaK::ConnectionStatus::Connecting:
        return tr("Выполняется подключение к трансиверу").toUpper();
    case PanoramaK::ConnectionStatus::Connected:
        return tr("Подключение к трансиверу выполнено").toUpper();
    default:
        return {};
    }
}

QString MainWindow::description(PanoramaK::Status status, bool details) const
{
    if (details) {
        switch (status) {
        case PanoramaK::Status::Ok:
            return tr("");
        case PanoramaK::Status::ValidSwr:
            return tr("АФУ полностью исправно, но рекомендуется выполнить согласование.").toUpper();
        case PanoramaK::Status::HighSwr:
            return tr("Не рекомендуется эксплуатация АФУ, необходимо выполнить согласование.").toUpper();
        case PanoramaK::Status::VeryHighSwr:
            return tr("Эксплуатация запрещена, требуется сервисное обслуживание АФУ!").toUpper();
        case PanoramaK::Status::CableBreakage:
            return tr("Эксплуатация запрещена, требуется сервисное обслуживание АФУ!").toUpper();
        case PanoramaK::Status::BadContact:
            return tr("Эксплуатация запрещена, требуется сервисное обслуживание АФУ!").toUpper();
        default:
            return tr("");
        }
    } else {
        switch (status) {
        case PanoramaK::Status::Ok: {
            lbAtuStatus->setStyleSheet("QLabel {color: green;}");
            return tr("Исправно.").toUpper();
        }
        case PanoramaK::Status::ValidSwr: {
            lbAtuStatus->setStyleSheet("QLabel {color: green;}");
            return tr("Допустимое значение КСВ: ").toUpper() + QString::number(m_swr, 'f', 1) + ".";
        }
        case PanoramaK::Status::HighSwr: {
            lbAtuStatus->setStyleSheet("QLabel {color: #FFD700;}");
            return tr("Высокий КСВ: ").toUpper() + QString::number(m_swr, 'f', 1) + "!";
        }
        case PanoramaK::Status::VeryHighSwr: {
            lbAtuStatus->setStyleSheet("QLabel {color: #FFA500;}");
            return tr("Очень высокий КСВ: ").toUpper() + QString::number(m_swr, 'f', 1) + "!";
        }
        case PanoramaK::Status::CableBreakage: {
            swrValueIndicatorWidget->setBackgroundColor(Qt::red);
            lbAtuStatus->setStyleSheet("QLabel {color: red;}");
            return tr("Обрыв кабеля!").toUpper();
        }
        case PanoramaK::Status::BadContact: {
            swrValueIndicatorWidget->setBackgroundColor(Qt::red);
            lbAtuStatus->setStyleSheet("QLabel {color: red;}");
            return tr("Плохой контакт!").toUpper();
        }
        default:
            return tr("Не определено.").toUpper();
        }
    }
}

QString MainWindow::description(bool trxMode) const
{
    if (trxMode) {
        lbTrxMode->setStyleSheet("QLabel {color: #FFA500;}");
        return tr("Режим передачи").toUpper();
    }

    lbTrxMode->setStyleSheet("QLabel {color: green;}");
    return tr("Режим приёма").toUpper();
}
