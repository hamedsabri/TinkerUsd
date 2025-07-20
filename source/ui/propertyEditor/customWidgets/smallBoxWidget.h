#pragma once

#include <QtWidgets/QWidget>
#include <QtGui/QColor>

namespace TINKERUSD_NS 
{

class SmallBoxWidget : public QWidget
{
    Q_OBJECT

public:
    SmallBoxWidget(const QSize& size, const QColor& color, QWidget *parent = nullptr);
    ~SmallBoxWidget() = default;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QSize  m_size;
    QColor m_color;
};

} // namespace TINKERUSD_NS