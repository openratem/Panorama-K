#pragma once

#include "../../control/ControlObserver.h"
#include <PowerSwrSeries.h>
#include <QJsonDocument>
#include <QObject>
#include <memory>
#include <string>
#include "LibLoader.h"

class Service final : public QObject {
    Q_OBJECT

public:
    explicit Service(QObject* parent = nullptr);

    ~Service();

    std::shared_ptr<ControlObserver> control();

    PowerSwrSeries<PowerSwrData>* series();

    DeviceInfo* getDevices();

signals:
    void currentSwr(float value);
    void currentWatts(float watts);
    void error(QString errorString);

private slots:
    void onProtocol(quint32 index);
    void onOpen(const QJsonObject& config, bool state);
    void onClose();

private:
    static void connectionStatus(int data, void* userData);

    static void status(int data, void* userData);

    static void frequency(int data, void* userData);

    static void trx(bool data, void* userData);

    static void badContact(void* userData);

    static void currentSwrValue(float watts, float swr, float filteredSwr, void* userData);

private:
    LibLoader m_loader;
    std::shared_ptr<ControlObserver> pControl;
    void* m_libService;
};
