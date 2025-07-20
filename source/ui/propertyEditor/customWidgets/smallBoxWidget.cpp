#include "smallBoxWidget.h"

#include <QtGui/QPainter>

namespace TINKERUSD_NS 
{

SmallBoxWidget::SmallBoxWidget(const QSize& size, const QColor& color, QWidget *parent)
    : QWidget(parent)
    , m_size(size)
    , m_color(color)
{
    setFixedSize(size);
}

void SmallBoxWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRect rect(0, 0, m_size.width(), m_size.height());
    painter.setBrush(QBrush(m_color));  // Fill color
    painter.drawRect(rect);
}

} // namespace TINKERUSD_NS