#include "valueEntryLineEdit.h"

#include <QApplication>
#include <QMouseEvent>
#include <cmath>

namespace TINKERUSD_NS
{

ValueEntryLineEdit::ValueEntryLineEdit(bool isDouble, QWidget* parent)
    : QLineEdit(parent)
    , m_isDouble(isDouble)
    , m_value(0.0)
    , m_decimals(isDouble ? 3 : 0)
    , m_stepSize(isDouble ? 0.01 : 1.0)
    , m_minimum(isDouble ? 0.0 : 0)
    , m_maximum(isDouble ? 1.0 : 100)
    , m_dragging(false)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    installEventFilter(this);
    connect(this, &QLineEdit::editingFinished, this, [&]() {
        bool ok = false;
        double enteredValue = text().toDouble(&ok);
        if (ok){
            setValue(enteredValue);
        }
        else {
            setText(QString::number(m_value, 'f', m_decimals));
        }
    });
    setValue(0.0);
}

void ValueEntryLineEdit::setStepSize(double stepSize)
{
    if (stepSize > 0) {
        m_stepSize = stepSize;
    }
}

void ValueEntryLineEdit::setValue(double value)
{
    m_value = std::clamp(value, m_minimum, m_maximum);
    setText(QString::number(m_value, 'f', m_decimals));
    emit valueChanged(m_value);
}

void ValueEntryLineEdit::setRange(double min, double max)
{
    m_minimum = min;
    m_maximum = max;
    setValue(m_value);
}

bool ValueEntryLineEdit::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == this) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton
                && QApplication::keyboardModifiers() & Qt::ControlModifier)
            {
                m_dragging = true;
                m_lastMousePos = mouseEvent->globalPos();
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove && m_dragging) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            double       delta = (mouseEvent->globalX() - m_lastMousePos.x()) * m_stepSize;
            setValue(m_value + delta);
            m_lastMousePos = mouseEvent->globalPos();
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            m_dragging = false;
            return true;
        }
    }
    return QLineEdit::eventFilter(obj, event);
}

} // namespace TINKERUSD_NS