#include "incrementalSlider.h"
#include "valueEntryLineEdit.h"

#include <QHBoxLayout>
#include <QStyle>
#include <QWheelEvent>

#include <algorithm>

namespace TINKERUSD_NS
{

static const float floatRadToDeg = 57.29578f;

void AttributeEditorSlider::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

void AttributeEditorSlider::mousePressEvent(QMouseEvent* event)
{
    // The default implementation of the slider doesn't consider clicking on the slider track (not the handle) as changing the value
    // Reimplementing this event forces this to be the case
    setSliderDown(true);
    QSlider::mousePressEvent(event);
}

FloatSliderGroup::FloatSliderGroup(float min, float max, float value, QWidget* parent)
    : QFrame(parent)
    , m_valueEntry(new DoubleValueLineEdit(this))
    , m_slider(new AttributeEditorSlider(this))
    , m_minimum(0.0f)
    , m_softMinimum(0.0f)
    , m_maximum(0.0f)
    , m_softMaximum(0.0f)
    , m_useSoftMinimum(false)
    , m_useSoftMaximum(false)
    , m_value(0.0f)
    , m_sliderBeginValue(0.0f)
    , m_decimals(3)
    , m_sliderFloatLimitWithDecimals(INT_MAX * powf(0.1f, m_decimals))
    , m_sliderValueConversionRatio(powf(10, m_decimals - 1))
{
    setupUI();

    connect(m_slider, SIGNAL(sliderPressed()), this, SLOT(onSliderPressed()));
    connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMoved(int)));
    connect(m_slider, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
    connect(m_valueEntry, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
    connect(m_valueEntry, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this, SIGNAL(contextMenuRequested(QContextMenuEvent*)));

    m_slider->setOrientation(Qt::Horizontal);
    m_valueEntry->setDecimals(m_decimals);

    setRange(min, max);
    setValue(value);
}

FloatSliderGroup::FloatSliderGroup(QWidget* parent)
    : FloatSliderGroup(0.0f, 1.0f, 1.0f, parent)
{
}

void FloatSliderGroup::setMinimum(float min)
{
    if (min >= m_maximum)
    {
        return;
    }

    m_minimum = min;
    if (!m_useSoftMinimum)
    {
        // Clamp min to a value so that after it is multiplied by (10 ^ m_decimals), the resulting value can still be stored in an integer
        // This means that the slider may be unable to cover the full range of values between min/max if they are too large
        // However you can still manually enter the numbers in the entry box for those numbers which are outside the range of the slider.

        const float sliderFloatMin = std::clamp(min, -m_sliderFloatLimitWithDecimals, m_sliderFloatLimitWithDecimals);
        const int sliderMinAdjusted = static_cast<int>(ceil(sliderFloatMin * m_sliderValueConversionRatio));

        m_slider->setMinimum(sliderMinAdjusted);
    }

    if (min > m_softMinimum)
    {
        setSoftMinimum(min);
    }

    m_valueEntry->setMinimum(min);

    if (m_value < m_minimum)
    {
        setValue(m_minimum);
    }
}

void FloatSliderGroup::setMaximum(float max)
{
    if (max <= m_minimum)
    {
        return;
    }

    m_maximum = max;
    if (!m_useSoftMaximum)
    {
        // Clamp max to a value so that after it is multiplied by (10 ^ m_decimals), the resulting value can still be stored in an integer
        // This means that the slider may be unable to cover the full range of values between min/max if they are too large
        // However you can still manually enter the numbers in the entry box for those numbers which are outside the range of the slider.

        const float sliderFloatMax = std::clamp(max, -m_sliderFloatLimitWithDecimals, m_sliderFloatLimitWithDecimals);
        const int sliderMaxAdjusted = static_cast<int>(floor(sliderFloatMax * m_sliderValueConversionRatio));

        m_slider->setMaximum(sliderMaxAdjusted);
    }

    if (max < m_softMaximum)
    {
        setSoftMaximum(max);
    }

    m_valueEntry->setMaximum(max);

    if (m_value > m_maximum)
    {
        setValue(m_maximum);
    }
}

void FloatSliderGroup::setSoftMinimum(float min)
{
    if (m_softMinimum == min || min > m_maximum || min > m_softMaximum)
    {
        return;
    }

    if (min > m_value)
    {
        min = m_value;
    }

    if (min < m_minimum)
    {
        min = m_minimum;
    }

    m_softMinimum = min;

    if (!m_useSoftMinimum)
    {
        return;
    }

    const int sliderMin = static_cast<int>(ceil(min * m_sliderValueConversionRatio));
    m_slider->setMinimum(sliderMin);
}

void FloatSliderGroup::setUseSoftMinimum(bool useSoftMin)
{
    if (useSoftMin == m_useSoftMinimum)
    {
        return;
    }

    m_useSoftMinimum = useSoftMin;

    if (!useSoftMin)
    {
        return;
    }

    if (m_softMinimum > m_value)
    {
        setSoftMinimum(m_value);
    }
    else if (m_softMinimum < m_minimum)
    {
        setSoftMinimum(m_minimum);
    }
    else
    {
        const int sliderMin = static_cast<int>(ceil(m_softMinimum * m_sliderValueConversionRatio));
        m_slider->setMinimum(sliderMin);
    }
}

void FloatSliderGroup::setSoftMaximum(float max)
{
    if (m_softMaximum == max || max < m_minimum || max < m_softMinimum)
    {
        return;
    }

    if (max < m_value)
    {
        max = m_value;
    }

    if (max > m_maximum)
    {
        max = m_maximum;
    }

    m_softMaximum = max;

    if (!m_useSoftMaximum)
    {
        return;
    }

    const int sliderMax = static_cast<int>(floor(max * m_sliderValueConversionRatio));
    m_slider->setMaximum(sliderMax);
}

void FloatSliderGroup::setUseSoftMaximum(bool useSoftMax)
{
    if (useSoftMax == m_useSoftMaximum)
    {
        return;
    }

    m_useSoftMaximum = useSoftMax;

    if (!useSoftMax)
    {
        return;
    }

    if (m_softMaximum < m_value)
    {
        setSoftMaximum(m_value);
    }
    else if (m_softMaximum > m_maximum)
    {
        setSoftMaximum(m_maximum);
    }
    else
    {
        const int sliderMax = static_cast<int>(floor(m_softMaximum * m_sliderValueConversionRatio));
        m_slider->setMaximum(sliderMax);
    }
}

void FloatSliderGroup::setRange(float min, float max)
{
    if (min >= max)
    {
        min = max - powf(0.1f, m_decimals);
    }

    setMinimum(min);
    setMaximum(max);
}

void FloatSliderGroup::setValue(float value)
{
    if (value == m_value)
    {
        return;
    }

    if (value > m_maximum)
    {
        value = m_maximum;
    }
    else if (value < m_minimum)
    {
        value = m_minimum;
    }

    // Adjust soft limits if necessary
    if (value < m_softMinimum)
    {
        if (m_useSoftMaximum)
        {
            setSoftMinimum(value - (m_softMaximum - value));
        }
        else
        {
            setSoftMinimum(value - (m_maximum - value));
        }
    }
    else if (value > m_softMaximum)
    {
        setSoftMaximum(value * 2.0f);
    }

    m_value = value;

    m_slider->blockSignals(true);
    m_valueEntry->blockSignals(true);
    
    const float sliderFloatValue = std::clamp(value, -m_sliderFloatLimitWithDecimals, m_sliderFloatLimitWithDecimals);
    const int sliderValueAdjusted = static_cast<int>(round(sliderFloatValue * m_sliderValueConversionRatio));

    m_slider->setValue(sliderValueAdjusted);
    m_valueEntry->setValue(value);

    m_slider->blockSignals(false);
    m_valueEntry->blockSignals(false);
}

void FloatSliderGroup::setDecimals(int32_t decimals)
{
    if (decimals == m_decimals)
    {
        return;
    }

    if (decimals < 1)
    {
        decimals = 1;
    }

    m_decimals = decimals;

    // Adjust the slider min/max values to accomodate the increased precision
    // As precision increases, the range the slider can accurately display decreases due to converting between integer and float values
    // However, the end user has the option to manually enter values that may be outside the slider's range

    m_sliderValueConversionRatio = powf(10, m_decimals - 1);
    m_sliderFloatLimitWithDecimals = INT_MAX / m_sliderValueConversionRatio;

    const float min = m_useSoftMinimum ? m_softMinimum : m_minimum;
    const float sliderMinFloatValue = std::clamp(min, -m_sliderFloatLimitWithDecimals, m_sliderFloatLimitWithDecimals);
    const int sliderMinAdjusted = static_cast<int>(ceil(sliderMinFloatValue * m_sliderValueConversionRatio));
    m_slider->setMinimum(sliderMinAdjusted);

    const float max = m_useSoftMaximum ? m_softMaximum : m_maximum;
    const float sliderMaxFloatValue = std::clamp(max, -m_sliderFloatLimitWithDecimals, m_sliderFloatLimitWithDecimals);
    const int sliderMaxAdjusted = static_cast<int>(floor(sliderMaxFloatValue * m_sliderValueConversionRatio));
    m_slider->setMaximum(sliderMaxAdjusted);
}

void FloatSliderGroup::onSliderMoved(int value)
{
    setValue(value / m_sliderValueConversionRatio);
    sliderMoved(m_value);
}

void FloatSliderGroup::onSpinBoxEditingFinished()
{
    const float previousValue = m_value;
    setValue(static_cast<float>(m_valueEntry->value()));
    if (previousValue != m_value)
    {
        valueUpdated(m_value);
    }
}

void FloatSliderGroup::setupUI()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    setLayout(layout);

    layout->addWidget(m_valueEntry);
    layout->addWidget(m_slider);
}

void FloatSliderGroup::setFieldWidth(int32_t fieldWidth)
{
    m_valueEntry->setFixedWidth(fieldWidth);
}

void FloatSliderGroup::onSliderPressed()
{
    m_sliderBeginValue = m_value;
    sliderPressed();
}

void FloatSliderGroup::onSliderReleased()
{
    sliderReleased();
    if (m_sliderBeginValue != m_value)
    {
        valueUpdated(m_value);
    }
}

IntSliderGroup::IntSliderGroup(int min, int max, int value, QWidget* parent)
    : QFrame(parent)
    , m_valueEntry(new IntValueLineEdit(this))
    , m_slider(new AttributeEditorSlider(this))
    , m_minimum(0)
    , m_softMinimum(0)
    , m_maximum(0)
    , m_softMaximum(0)
    , m_useSoftMinimum(false)
    , m_useSoftMaximum(false)
    , m_value(0)
{
    setupUI();

    connect(m_slider, SIGNAL(sliderPressed()), this, SLOT(onSliderPressed()));
    connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMoved(int)));
    connect(m_slider, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
    connect(m_valueEntry, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
    connect(m_valueEntry, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this, SIGNAL(contextMenuRequested(QContextMenuEvent*)));

    setRange(min, max);
    setValue(value);
}

IntSliderGroup::IntSliderGroup(QWidget* parent)
    : IntSliderGroup(0, 1, 1, parent)
{
}

void IntSliderGroup::setMinimum(int min)
{
    if (min >= m_maximum){
        return;
    }

    m_minimum = min;
    if (!m_useSoftMinimum){
        m_slider->setMinimum(min);
    }

    if (min > m_softMinimum){
        setSoftMinimum(min);
    }

    m_valueEntry->setMinimum(min);

    if (m_value < m_minimum){
        setValue(m_minimum);
    }
}

void IntSliderGroup::setSoftMinimum(int min)
{
    if (m_softMinimum == min || min > m_maximum || min > m_softMaximum){
        return;
    }

    if (min > m_value) {
        min = m_value;
    }

    if (min < m_minimum) {
        min = m_minimum;
    }

    m_softMinimum = min;

    if (!m_useSoftMinimum) {
        return;
    }

    m_slider->setMinimum(min);
}

void IntSliderGroup::setUseSoftMinimum(bool useSoftMin)
{
    if (useSoftMin == m_useSoftMinimum) {
        return;
    }

    m_useSoftMinimum = useSoftMin;

    if (!useSoftMin) {
        return;
    }

    if (m_softMinimum > m_value) {
        setSoftMinimum(m_value);
    }
    else if (m_softMinimum < m_minimum) {
        setSoftMinimum(m_minimum);
    }
    else {
        m_slider->setMinimum(m_softMinimum);
    }
}

void IntSliderGroup::setMaximum(int max)
{
    if (max <= m_minimum) {
        return;
    }

    m_maximum = max;
    if (!m_useSoftMaximum) {
        m_slider->setMaximum(max);
    }

    if (max < m_softMaximum) {
        setSoftMaximum(max);
    }

    m_valueEntry->setMaximum(max);

    if (m_value > m_maximum) {
        setValue(m_maximum);
    }
}

void IntSliderGroup::setSoftMaximum(int max)
{
    if (m_softMaximum == max || max < m_minimum || max < m_softMinimum) {
        return;
    }

    if (max < m_value) {
        max = m_value;
    }

    if (max > m_maximum) {
        max = m_maximum;
    }

    m_softMaximum = max;

    if (!m_useSoftMaximum) {
        return;
    }

    m_slider->setMaximum(max);
}

void IntSliderGroup::setUseSoftMaximum(bool useSoftMax)
{
    if (useSoftMax == m_useSoftMaximum)
    {
        return;
    }

    m_useSoftMaximum = useSoftMax;

    if (!useSoftMax) {
        return;
    }

    if (m_softMaximum < m_value) {
        setSoftMaximum(m_value);
    }
    else if (m_softMaximum > m_maximum) {
        setSoftMaximum(m_maximum);
    }
    else {
        m_slider->setMaximum(m_softMaximum);
    }
}

void IntSliderGroup::setRange(int min, int max)
{
    if (min >= max)
    {
        min = max - 1;
    }

    setMinimum(min);
    setMaximum(max);
}

void IntSliderGroup::setValue(int value)
{
    if (value == m_value)
    {
        return;
    }

    if (value > m_maximum)
    {
        value = m_maximum;
    }
    else if (value < m_minimum)
    {
        value = m_minimum;
    }

    // Adjust soft limits if necessary
    if (value < m_softMinimum)
    {
        if (m_useSoftMaximum)
        {
            setSoftMinimum(value - (m_softMaximum - value));
        }
        else
        {
            setSoftMinimum(value - (m_maximum - value));
        }
    }
    else if (value > m_softMaximum)
    {
        setSoftMaximum(value * 2.0f);
    }

    m_value = value;

    m_slider->blockSignals(true);
    m_valueEntry->blockSignals(true);

    m_slider->setValue(value);
    m_valueEntry->setValue(value);

    m_slider->blockSignals(false);
    m_valueEntry->blockSignals(false);
}

void IntSliderGroup::onSliderMoved(int value)
{
    setValue(value);
    sliderMoved(value);
}

void IntSliderGroup::onSpinBoxEditingFinished()
{
    const float previousValue = m_value;
    setValue(m_valueEntry->value());
    if (previousValue != m_value)
    {
        valueUpdated(m_value);
    }
}

void IntSliderGroup::setupUI()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    setLayout(layout);

    m_slider->setOrientation(Qt::Horizontal);

    layout->addWidget(m_valueEntry);
    layout->addWidget(m_slider);
}

void IntSliderGroup::setFieldWidth(int32_t fieldWidth)
{
    m_valueEntry->setFixedWidth(fieldWidth);
}

void IntSliderGroup::onSliderPressed()
{
    m_sliderBeginValue = m_value;
    sliderPressed();
}

void IntSliderGroup::onSliderReleased()
{
    sliderReleased();
    if (m_sliderBeginValue != m_value)
    {
        valueUpdated(m_value);
    }
}

} // namespace TINKERUSD_NS