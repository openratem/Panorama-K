#ifndef QUEUEEVENT_H
#define QUEUEEVENT_H

#include <queue>
#include <atomic>
#include <stdint.h>
#include <QtCore>

namespace PanoramaK {

using namespace std;

class QueueEvent : public QObject
{
    Q_OBJECT

public:
    explicit QueueEvent(QObject *parent = nullptr) : QObject(parent)
    {
        connect(this, &QueueEvent::readyRead, [=]()
        { m_mutex.lock(); m_loop.quit(); m_mutex.unlock(); });
    }

    uint32_t count() const noexcept
    {
        QMutexLocker t_locker(const_cast<QMutex*>(&m_mutex));
        return m_queue.count();
    }

    void push(const QByteArray &data)
    {
        m_mutex.lock();
            m_queue.enqueue(data);
        m_mutex.unlock();
        emit readyRead();
    }

    bool pop(QByteArray &data)
    {
        QMutexLocker t_locker(const_cast<QMutex*>(&m_mutex));
        if (m_queue.count() < 1)
            return false;

        data = m_queue.dequeue();
        return true;
    }

    bool waitForReadyRead(uint32_t timeoutMs)
    {
        m_mutex.lock();
        if (m_queue.count() > 0) {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();

        QTimer::singleShot(timeoutMs, [=]()
        { m_mutex.lock(); m_loop.quit(); m_mutex.unlock(); });

        m_loop.exec();

        QMutexLocker t_locker(const_cast<QMutex*>(&m_mutex));
        return (m_queue.count() > 0);
    }

signals:
    void readyRead();
    void startWait();

private:
    Q_DISABLE_COPY(QueueEvent)

private:
    QQueue<QByteArray> m_queue;
    QEventLoop m_loop;
    QMutex     m_mutex;
};

}

#endif // QUEUEEVENT_H
