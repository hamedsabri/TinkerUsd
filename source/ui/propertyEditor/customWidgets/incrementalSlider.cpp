#include "incrementalSlider.h"

#include <cmath>
#include <QHBoxLayout>
#include <QSignalBlocker>

namespace TINKERUSD_NS
{

IncrementalSliderGroup::IncrementalSliderGroup(bool isDouble, QWidget* parent)
    : QWidget(parent)
    , m_isDouble(isDouble)
    , m_slider(new QSlider(Qt::Horizontal, this))
    , m_valueEntry(new ValueEntryLineEdit(isDouble, this))
    , m_minimum(isDouble ? 0.0 : 0)
    , m_maximum(isDouble ? 1.0 : 100)
    , m_value(isDouble ? 0.0 : 0)
    , m_stepSize(isDouble ? 0.01 : 1)
{
    setupUI();
    setRange(m_minimum, m_maximum);
    setValue(m_value);
}

void IncrementalSliderGroup::setRange(double min, double max)
{
    m_minimum = min;
    m_maximum = max;
    m_slider->setRange(static_cast<int>(min / m_stepSize), static_cast<int>(max / m_stepSize));
    m_valueEntry->setRange(min, max);
    setValue(m_value);
}

void IncrementalSliderGroup::setValue(double value)
{
    m_value = std::clamp(value, m_minimum, m_maximum);
    m_slider->setValue(static_cast<int>(m_value / m_stepSize));
    {
        QSignalBlocker blocker(m_valueEntry); // Prevent valueChanged emission
        m_valueEntry->setValue(m_value);
    }
    emit valueChanged(m_value);
}

void IncrementalSliderGroup::setStepSize(double stepSize)
{
    if (stepSize > 0)
    {
        m_stepSize = stepSize;
        m_valueEntry->setStepSize(stepSize);
        setRange(m_minimum, m_maximum);
    }
}

void IncrementalSliderGroup::setupUI()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    layout->addWidget(m_valueEntry);
    layout->addWidget(m_slider);
    setLayout(layout);

    connect(m_slider, &QSlider::valueChanged, this, [&](int sliderValue) {
        setValue(sliderValue * m_stepSize);
    });

    connect(m_valueEntry, &ValueEntryLineEdit::valueChanged, this, [&](double value) {
        setValue(value);
    });
}

} // namespace TINKERUSD_NS