#pragma once

#include <string_view>
#include <QSerialPort>
#include "../../PanoramaK.h"

namespace PanoramaK {

class CatAnalizer final : public PowerSwrAnalizerAbstract
{
    Q_OBJECT

public:
    explicit CatAnalizer(QObject *parent = nullptr);

    /**
     * \brief Открывает COM порт для чтения показаний мощности и КСВ.
     * \param config - настройки COM порта.
     * \return \a true если порт открыт, иначе \a false.
     */
    bool open(const QJsonObject &config);

    /**
     * \brief Закрывает COM порт.
     */
    void close();

    /**
     * \brief Возвращает текущее значение мощности, Вт.
     */
    float power() const override;

    /**
     * \brief Возвращает текущее значение КСВ.
     */
    float swr() const override;

private slots:
    void parse(std::string_view text);
    void onPowerSwr(float watts, float swr);

private:
    QSerialPort m_serialPort;
    float m_power {0.0f};
    float m_swr {1.0f};
    std::string m_text;
};

} // namespace PanoramaK
