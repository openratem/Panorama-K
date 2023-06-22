#include "MainWindow.h"
#include <memory>
#include <QtSerialPort>

MainWindow::MainWindow(PowerSwrSeries<PowerSwrData> &series, QWidget *parent) :
    QMainWindow{parent}, m_powerSwrSeries{series}
{
    setupUi(this);
    initialize();
}

void MainWindow::initialize()
{
    pControl = std::make_shared<ControlObserver>();

    //
    pGraphics = std::make_unique<Graphics>(m_powerSwrSeries).release();
    pLayoutService->addWidget(pGraphics);

    // загрузка списка поддерживаемых трансиверов
    for (const auto &protocol : PanoramaK::TransceiverFactory::devices())
        cbTransceivers->addItem(protocol.name, static_cast<int>(protocol.interface));

    // получение доступных COM портов
    for (const auto &info : QSerialPortInfo::availablePorts()) {
        cbComportName->addItem(info.portName());
        cbMeterComportName->addItem(info.portName());
    }

    // настройка системы событий
    connect(pControl.get(), &ControlObserver::protocolChanged, cbTransceivers, &QComboBox::setCurrentIndex);
    connect(pControl.get(), &ControlObserver::connectionStatusChanged, this, &MainWindow::onConnectionStatus);
    connect(pControl.get(), &ControlObserver::trxChanged, pGraphics, &Graphics::setAnalize);
    connect(pControl.get(), &ControlObserver::trxChanged, this, &MainWindow::onTrxMode);
    connect(pControl.get(), &ControlObserver::statusChanged, this, &MainWindow::onStatus);
    connect(cbTransceivers, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onProtocol);
    connect(pbNetworkConnect, &QPushButton::toggled, this, &MainWindow::onOpen);
    connect(pbSerialPortOpen, &QPushButton::toggled, this, &MainWindow::onOpen);

    // начальная инициализация
    tabWidget->setCurrentIndex(0);

    onProtocol(cbTransceivers->currentIndex());
    onStatus(pControl->status());
    onConnectionStatus(pControl->connectionStatus());
    onTrxMode(pControl->trx());
}

void MainWindow::setState(const QJsonObject &obj)
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

    //
    if (state)
        pControl->open(connectionSettings());
    else
        pControl->close();
}

QJsonObject MainWindow::connectionSettings() const
{
    QJsonObject t_config;
    const auto t_interface = static_cast<PanoramaK::ControlInterface>(cbTransceivers->currentData().toInt());

    if (t_interface == PanoramaK::ControlInterface::Network) {
        QJsonObject t_network;
        t_network["address"] = leHostAddress->text();

        t_config["network"] = t_network;
    }
    else if (t_interface == PanoramaK::ControlInterface::SerialPort) {
        QJsonObject t_serialPort;
        t_serialPort["name"]     = cbComportName->currentText();
        t_serialPort["parity"]   = cbComportParity->currentText();
        t_serialPort["data"]     = cbComportData->currentText().toInt();
        t_serialPort["baudrate"] = cbComportBaudrate->currentText().toInt();
        t_serialPort["dtr"]      = cbComportDtr->currentIndex() == 1 ? 1 : 0;
        t_serialPort["rts"]      = cbComportRts->currentIndex() == 1 ? 1 : 0;

        t_config["serialPort"] = t_serialPort;

        QJsonObject t_serialPortAnalizer;
        t_serialPortAnalizer["name"]     = cbMeterComportName->currentText();
        t_serialPortAnalizer["parity"]   = "Нет";
        t_serialPortAnalizer["data"]     = "8";
        t_serialPortAnalizer["baudrate"] = "9600";
        t_serialPortAnalizer["dtr"]      = 0;
        t_serialPortAnalizer["rts"]      = 0;

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
    }
    else if (status == PanoramaK::ConnectionStatus::Connected) {
        QString t_title = tr("Подключено");
        pbSerialPortOpen->setText(t_title);
        pbNetworkConnect->setText(t_title);
    }
    else {
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
        case PanoramaK::ConnectionStatus::Disconnect: return tr("Выполните подключение к трансиверу");
        case PanoramaK::ConnectionStatus::Connecting: return tr("Выполняется подключение к трансиверу");
        case PanoramaK::ConnectionStatus::Connected: return tr("Подключение к трансиверу выполнено");
        default: return {};
    }
}

QString MainWindow::description(PanoramaK::Status status, bool details) const
{
    if (details) {
        switch (status) {
            case PanoramaK::Status::Ok: return tr("");
            case PanoramaK::Status::ValidSwr: return tr("АФУ полностью исправно, но рекомендуется выполнить согласование.");
            case PanoramaK::Status::HighSwr: return tr("Не рекомендуется эксплуатация АФУ, необходимо выполнить согласование.");
            case PanoramaK::Status::VeryHighSwr: return tr("Эксплуатация запрещена, требуется сервисное обслуживание АФУ!");
            case PanoramaK::Status::CableBreakage: return tr("Эксплуатация запрещена, требуется сервисное обслуживание АФУ!");
            case PanoramaK::Status::BadContact: return tr("Эксплуатация запрещена, требуется сервисное обслуживание АФУ!");
            default: return tr("");
        }
    }
    else {
        switch (status) {
            case PanoramaK::Status::Ok: return tr("Состояние АФУ: исправно.");
            case PanoramaK::Status::ValidSwr: return tr("Состояние АФУ: допустимое значение КСВ: ") + QString::number(m_swr, 'f', 1) + ".";
            case PanoramaK::Status::HighSwr: return tr("Состояние АФУ: высокое КСВ: ") + QString::number(m_swr, 'f', 1) + "!";
            case PanoramaK::Status::VeryHighSwr: return tr("Состояние АФУ: очень высокое КСВ: ") + QString::number(m_swr, 'f', 1) + "!";
            case PanoramaK::Status::CableBreakage: return tr("Состояние АФУ: обрыв кабеля!");
            case PanoramaK::Status::BadContact: return tr("Состояние АФУ: плохой контакт!");
            default: return tr("Состояние АФУ не определено.");
        }
    }
}

QString MainWindow::description(bool trxMode) const
{
    return trxMode ? tr("Режим передачи") : tr("Режим приёма");
}









