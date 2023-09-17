#pragma once

#include <QWidget>

class ValueIndicator : public QWidget {
    Q_OBJECT
public:
    explicit ValueIndicator(QWidget* parent = nullptr);

    void setValue(double value);
    void setPreffix(QString preffix);
    void setSuffix(QString suffix);
    void setBackgroundColor(QColor color);

signals:
    void valueChanged(double);
    void backgroundColorChanged(QColor);

private:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_value { 0.0 };
    QColor m_color { "#008000" };

    QString m_preffix { "" };
    QString m_suffix { "" };
};
