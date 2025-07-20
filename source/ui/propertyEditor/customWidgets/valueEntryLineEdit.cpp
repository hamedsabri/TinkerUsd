#include "valueEntryLineEdit.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QSizePolicy>
#include <QtCore/QTimer>

namespace TINKERUSD_NS
{
DoubleValueLineEdit::DoubleValueLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , m_validator(new QDoubleValidator(this))
    , m_minimum(0.0)
    , m_maximum(1.0)
    , m_value(0.0)
    , m_decimals(3)
{
    setValidator(m_validator);
    setValue(1.0);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    connect(this, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

void DoubleValueLineEdit::setMinimum(double min)
{
    if (min == m_minimum)
    {
        return;
    }

    if (min >= m_maximum)
    {
        min = m_maximum - std::pow(0.1, m_decimals);
    }

    m_minimum = min;
    if (m_value < min)
    {
        setValue(min);
    }
}

void DoubleValueLineEdit::setMaximum(double max)
{
    if (max == m_maximum)
    {
        return;
    }

    if (max <= m_minimum)
    {
        max = m_minimum + pow(0.1, m_decimals);
    }

    m_maximum = max;
    if (m_value > max)
    {
        setValue(max);
    }
}

void DoubleValueLineEdit::setRange(double min, double max)
{
    setMinimum(min);
    setMaximum(max);
}

void DoubleValueLineEdit::setDecimals(int32_t decimals)
{
    m_decimals = decimals;
}

void DoubleValueLineEdit::setValue(double value)
{
    value = std::min(std::max(value, m_minimum), m_maximum);

    // Update the display text to show the appropriate number of 0s
    QString numberText = QString::number(value, 'f', m_decimals);
    if (!numberText.contains('.'))
    {
        numberText += ".";
    }

    const Qt::SplitBehavior splitBehavior = Qt::KeepEmptyParts;

    while (numberText.split('.', splitBehavior)[1].size() < m_decimals)
    {
        numberText += "0";
    }

    const bool areSignalsAlreadyBlocked = signalsBlocked();
    blockSignals(true);
    setText(numberText);
    blockSignals(areSignalsAlreadyBlocked);

    if (numberText == m_previouslyValidatedText)
    {
        return;
    }
    m_previouslyValidatedText = numberText;

    if (value != m_value)
    {
        m_value = value;
        valueChanged(m_value);
    }
}

void DoubleValueLineEdit::onEditingFinished()
{
    validateText();
}

void DoubleValueLineEdit::onReturnPressed()
{
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void DoubleValueLineEdit::validateText()
{
    bool success = false;
    const double value = text().toDouble(&success);
    if (!success)
    {
        setValue(m_value);
        return;
    }

    setValue(value);
}

void DoubleValueLineEdit::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);

    event->accept();

    if (text().length() > 0)
    {
        return;
    }

    if (event->key() != Qt::Key_Enter && event->key() != Qt::Key_Return)
    {
        return;
    }

    if (event->modifiers() != 0)
    {
        return;
    }

    editingFinished();
    returnPressed();
}

void DoubleValueLineEdit::contextMenuEvent(QContextMenuEvent* event)
{
    contextMenuRequested(event);
    event->accept();
    return;
}


IntValueLineEdit::IntValueLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , m_validator(new QIntValidator(this))
    , m_minimum(0)
    , m_maximum(1)
    , m_value(0)
{
    setValidator(m_validator);
    setValue(1);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    connect(this, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

void IntValueLineEdit::setMinimum(int32_t min)
{
    if (min == m_minimum)
    {
        return;
    }

    if (min >= m_maximum)
    {
        min = m_maximum - 1;
    }

    m_minimum = min;
    if (m_value < min)
    {
        setValue(min);
    }
}

void IntValueLineEdit::setMaximum(int32_t max)
{
    if (max == m_maximum)
    {
        return;
    }

    if (max <= m_minimum)
    {
        max = m_minimum + 1;
    }

    m_maximum = max;
    if (m_value > max)
    {
        setValue(max);
    }
}

void IntValueLineEdit::setRange(int32_t min, int32_t max)
{
    setMinimum(min);
    setMaximum(max);
}

void IntValueLineEdit::setValue(int32_t value)
{
    const bool areSignalsAlreadyBlocked = signalsBlocked();
    blockSignals(true);
    value = std::min(std::max(value, m_minimum), m_maximum);

    setText(QString::number(value));
    blockSignals(areSignalsAlreadyBlocked);

    if (value != m_value)
    {
        m_value = value;
        valueChanged(m_value);
    }
}

void IntValueLineEdit::onEditingFinished()
{
    validateText();
}

void IntValueLineEdit::onReturnPressed()
{
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void IntValueLineEdit::validateText()
{
    bool success = false;
    const double value = text().toDouble(&success);
    if (!success)
    {
        return;
    }

    setValue(value);
}

void IntValueLineEdit::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);
    event->accept();

    if (text().length() > 0)
    {
        return;
    }

    if (event->key() != Qt::Key_Enter && event->key() != Qt::Key_Return)
    {
        return;
    }

    if (event->modifiers() != 0)
    {
        return;
    }

    editingFinished();
    returnPressed();
}

void IntValueLineEdit::contextMenuEvent(QContextMenuEvent* event)
{
    contextMenuRequested(event);
    event->accept();
    return;
}

} // namespace TINKERUSD_NS