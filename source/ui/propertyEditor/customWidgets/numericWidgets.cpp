#include "numericWidgets.h"

#include <QtCore/QTimer>
#include <QtWidgets/QHBoxLayout>

namespace TINKERUSD_NS
{

ValueLineEditBase::ValueLineEditBase(QWidget* parent)
    : QLineEdit(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(this, &QLineEdit::editingFinished, this, &ValueLineEditBase::onEditingFinished);
    connect(this, &QLineEdit::returnPressed, this, &ValueLineEditBase::onReturnPressed);
}

void ValueLineEditBase::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);
    event->accept();

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        if (event->modifiers() == Qt::NoModifier)
        {
            editingFinished();
            returnPressed();
        }
    }
}

template <typename T> void ValueLineEdit<T>::validateText()
{
    QString text = this->text().trimmed();
    if (text.isEmpty())
    {
        setValue(QVariant::fromValue(m_minimum));
        return;
    }

    bool success = false;
    T    value;
    if constexpr (std::is_same_v<T, int32_t>)
    {
        value = static_cast<T>(text.toInt(&success));
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        value = static_cast<T>(text.toUInt(&success));
    }
    else
    {
        value = static_cast<T>(text.toDouble(&success));
    }

    if (!success)
    {
        setValue(QVariant::fromValue(m_value));
        return;
    }

    setValue(QVariant::fromValue(value));
}

void ValueLineEditBase::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuRequested(event);
    event->accept();
}

void ValueLineEditBase::onEditingFinished() { validateText(); }

void ValueLineEditBase::onReturnPressed() { QTimer::singleShot(0, this, &QLineEdit::selectAll); }

template <typename T>
ValueLineEdit<T>::ValueLineEdit(QWidget* parent)
    : ValueLineEditBase(parent)
    , m_validator(
          std::is_integral_v<T> ? static_cast<QValidator*>(new QIntValidator(this))
                                : static_cast<QValidator*>(new QDoubleValidator(this)))
    , m_minimum(T(0))
    , m_maximum(T(1))
    , m_value(T(0))
{
    setValidator(m_validator);
    setValue(QVariant::fromValue(T(1)));
}

template <typename T> void ValueLineEdit<T>::setMinimum(T min)
{
    if (min == m_minimum)
        return;

    if constexpr (std::is_floating_point_v<T>)
    {
        if (min >= m_maximum)
            min = m_maximum - std::pow(0.1, m_decimals);
    }
    else
    {
        if (min >= m_maximum)
            min = m_maximum - 1;
    }

    m_minimum = min;
    if (m_value < min)
        setValue(QVariant::fromValue(min));

    if constexpr (std::is_integral_v<T>)
    {
        static_cast<QIntValidator*>(m_validator)->setBottom(min);
    }
    else
    {
        static_cast<QDoubleValidator*>(m_validator)->setBottom(min);
        static_cast<QDoubleValidator*>(m_validator)->setDecimals(m_decimals);
    }
}

template <typename T> void ValueLineEdit<T>::setMaximum(T max)
{
    if (max == m_maximum)
        return;

    if constexpr (std::is_floating_point_v<T>)
    {
        if (max <= m_minimum)
            max = m_minimum + std::pow(0.1, m_decimals);
    }
    else
    {
        if (max <= m_minimum)
            max = m_minimum + 1;
    }

    m_maximum = max;
    if (m_value > max)
        setValue(QVariant::fromValue(max));

    if constexpr (std::is_integral_v<T>)
    {
        static_cast<QIntValidator*>(m_validator)->setTop(max);
    }
    else
    {
        static_cast<QDoubleValidator*>(m_validator)->setTop(max);
    }
}

template <typename T> void ValueLineEdit<T>::setRange(T min, T max)
{
    setMinimum(min);
    setMaximum(max);
}

template <typename T> void ValueLineEdit<T>::setDecimals(int32_t decimals)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        m_decimals = decimals;
        static_cast<QDoubleValidator*>(m_validator)->setDecimals(decimals);
        setValue(QVariant::fromValue(m_value)); // Update display with new precision
    }
}

template <typename T> void ValueLineEdit<T>::setValue(const QVariant& value)
{
    T val = value.value<T>();
    val = std::min(std::max(val, m_minimum), m_maximum);

    QString numberText = QString::number(val);
    if constexpr (std::is_floating_point_v<T>)
    {
        numberText = QString::number(val, 'f', m_decimals);
        if (!numberText.contains('.'))
            numberText += ".";

        const Qt::SplitBehavior splitBehavior = Qt::KeepEmptyParts;
        while (numberText.split('.', splitBehavior)[1].size() < m_decimals)
            numberText += "0";
    }

    const bool signalsBlocked = this->signalsBlocked();
    blockSignals(true);
    setText(numberText);
    blockSignals(signalsBlocked);

    if (numberText == m_previouslyValidatedText)
        return;

    m_previouslyValidatedText = numberText;

    if (val != m_value)
    {
        m_value = val;
        emit valueChanged(QVariant::fromValue(m_value));
    }
}

SliderGroupBase::SliderGroupBase(QWidget* parent)
    : QFrame(parent)
{
}

template <typename T>
SliderGroup<T>::SliderGroup(T min, T max, T value, QWidget* parent)
    : SliderGroupBase(parent)
    , m_valueEntry(new ValueLineEdit<T>(this))
    , m_slider(new AttributeEditorSliderClass(this))
    , m_minimum(min)
    , m_softMinimum(min)
    , m_maximum(max)
    , m_softMaximum(max)
    , m_value(value)
    , m_sliderBeginValue(value)
{
    setupUI();
    connect(m_slider, &QSlider::sliderPressed, this, &SliderGroup::onSliderPressed);
    connect(m_slider, &QSlider::sliderMoved, this, &SliderGroup::onSliderMoved);
    connect(m_slider, &QSlider::sliderReleased, this, &SliderGroup::onSliderReleased);
    connect(m_valueEntry, &ValueLineEditBase::valueChanged, this, &SliderGroup::onSpinBoxEditingFinished);
    connect(m_valueEntry, &ValueLineEditBase::contextMenuRequested, this, &SliderGroup::contextMenuRequested);

    setRange(min, max);
    setValue(QVariant::fromValue(value));
}

template <typename T>
SliderGroup<T>::SliderGroup(QWidget* parent)
    : SliderGroup(T(0), T(1), T(1), parent)
{
}

template <typename T> void SliderGroup<T>::setupUI()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    m_slider->setOrientation(Qt::Horizontal);
    layout->addWidget(m_valueEntry);
    layout->addWidget(m_slider);
}

template <typename T> void SliderGroup<T>::setMinimum(T min)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        if (min >= m_maximum)
            return;
    }
    else
    {
        if (min >= m_maximum)
            return;
    }

    m_minimum = min;
    if (!m_useSoftMinimum)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            const T sliderFloatMin = std::clamp(
                min,
                static_cast<T>(-m_sliderFloatLimitWithDecimals),
                static_cast<T>(m_sliderFloatLimitWithDecimals));
            m_slider->setMinimum(static_cast<int>(std::ceil(sliderFloatMin * m_sliderValueConversionRatio)));
        }
        else
        {
            m_slider->setMinimum(min);
        }
    }

    if (min > m_softMinimum)
        setSoftMinimum(min);

    m_valueEntry->setMinimum(min);

    if (m_value < m_minimum)
        setValue(QVariant::fromValue(m_minimum));
}

template <typename T> void SliderGroup<T>::setMaximum(T max)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        if (max <= m_minimum)
            return;
    }
    else
    {
        if (max <= m_minimum)
            return;
    }

    m_maximum = max;
    if (!m_useSoftMaximum)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            const T sliderFloatMax = std::clamp(
                max,
                static_cast<T>(-m_sliderFloatLimitWithDecimals),
                static_cast<T>(m_sliderFloatLimitWithDecimals));
            m_slider->setMaximum(static_cast<int>(std::floor(sliderFloatMax * m_sliderValueConversionRatio)));
        }
        else
        {
            m_slider->setMaximum(max);
        }
    }

    if (max < m_softMaximum)
        setSoftMaximum(max);

    m_valueEntry->setMaximum(max);

    if (m_value > m_maximum)
        setValue(QVariant::fromValue(m_maximum));
}

template <typename T> void SliderGroup<T>::setRange(T min, T max)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        if (min >= max)
        {
            min = max - std::pow(0.1, m_decimals);
        }
    }
    else
    {
        if (min >= max)
        {
            min = max - 1;
        }
    }

    setMinimum(min);
    setMaximum(max);
}

template <typename T> void SliderGroup<T>::setSoftMinimum(T min)
{
    if (min == m_softMinimum || min > m_maximum || min > m_softMaximum)
        return;

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

    if constexpr (std::is_floating_point_v<T>)
    {
        m_slider->setMinimum(static_cast<int>(std::ceil(min * m_sliderValueConversionRatio)));
    }
    else
    {
        m_slider->setMinimum(min);
    }
}

template <typename T> void SliderGroup<T>::setSoftMaximum(T max)
{
    if (max == m_softMaximum || max < m_minimum || max < m_softMinimum)
        return;

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

    if constexpr (std::is_floating_point_v<T>)
    {
        m_slider->setMaximum(static_cast<int>(std::floor(max * m_sliderValueConversionRatio)));
    }
    else
    {
        m_slider->setMaximum(max);
    }
}

template <typename T> void SliderGroup<T>::setUseSoftMinimum(bool useSoftMin)
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
    else if constexpr (std::is_floating_point_v<T>)
    {
        m_slider->setMinimum(static_cast<int>(std::ceil(m_softMinimum * m_sliderValueConversionRatio)));
    }
    else
    {
        m_slider->setMinimum(m_softMinimum);
    }
}

template <typename T> void SliderGroup<T>::setUseSoftMaximum(bool useSoftMax)
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
    else if constexpr (std::is_floating_point_v<T>)
    {
        m_slider->setMaximum(static_cast<int>(std::floor(m_softMaximum * m_sliderValueConversionRatio)));
    }
    else
    {
        m_slider->setMaximum(m_softMaximum);
    }
}

template <typename T> void SliderGroup<T>::setDecimals(int32_t decimals)
{
    if constexpr (std::is_floating_point_v<T>)
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
        m_valueEntry->setDecimals(decimals);

        m_sliderValueConversionRatio = std::pow(10.0, m_decimals - 1);
        m_sliderFloatLimitWithDecimals = INT_MAX / m_sliderValueConversionRatio;

        const T min = m_useSoftMinimum ? m_softMinimum : m_minimum;
        const T sliderMinFloatValue = std::clamp(
            min,
            static_cast<T>(-m_sliderFloatLimitWithDecimals),
            static_cast<T>(m_sliderFloatLimitWithDecimals));
        m_slider->setMinimum(static_cast<int>(std::ceil(sliderMinFloatValue * m_sliderValueConversionRatio)));

        const T max = m_useSoftMaximum ? m_softMaximum : m_maximum;
        const T sliderMaxFloatValue = std::clamp(
            max,
            static_cast<T>(-m_sliderFloatLimitWithDecimals),
            static_cast<T>(m_sliderFloatLimitWithDecimals));
        m_slider->setMaximum(
            static_cast<int>(std::floor(sliderMaxFloatValue * m_sliderValueConversionRatio)));
    }
}

template <typename T> void SliderGroup<T>::setValue(const QVariant& value)
{
    T val = value.value<T>();
    if (val == m_value)
    {
        return;
    }

    if (val > m_maximum)
    {
        val = m_maximum;
    }
    else if (val < m_minimum)
    {
        val = m_minimum;
    }
    if (val < m_softMinimum)
    {
        if (m_useSoftMaximum)
        {
            setSoftMinimum(val - (m_softMaximum - val));
        }
        else
        {
            setSoftMinimum(val - (m_maximum - val));
        }
    }
    else if (val > m_softMaximum)
    {
        setSoftMaximum(val * 2);
    }

    m_value = val;

    m_slider->blockSignals(true);
    m_valueEntry->blockSignals(true);

    if constexpr (std::is_floating_point_v<T>)
    {
        const T sliderFloatValue = std::clamp(
            val,
            static_cast<T>(-m_sliderFloatLimitWithDecimals),
            static_cast<T>(m_sliderFloatLimitWithDecimals));
        m_slider->setValue(static_cast<int>(std::round(sliderFloatValue * m_sliderValueConversionRatio)));
    }
    else
    {
        m_slider->setValue(val);
    }

    m_valueEntry->setValue(QVariant::fromValue(val));

    m_slider->blockSignals(false);
    m_valueEntry->blockSignals(false);

    emit valueUpdated(QVariant::fromValue(val));
}

template <typename T> void SliderGroup<T>::onSliderPressed()
{
    m_sliderBeginValue = m_value;
    emit sliderPressed();
}

template <typename T> void SliderGroup<T>::onSliderMoved(int value)
{
    T newValue = value;
    if constexpr (std::is_floating_point_v<T>)
    {
        newValue = static_cast<T>(value) / m_sliderValueConversionRatio;
    }

    setValue(QVariant::fromValue(newValue));
    emit sliderMoved(QVariant::fromValue(newValue));
}

template <typename T> void SliderGroup<T>::onSliderReleased()
{
    emit sliderReleased();
    if (m_sliderBeginValue != m_value)
    {
        emit valueUpdated(QVariant::fromValue(m_value));
    }
}

template <typename T> void SliderGroup<T>::onSpinBoxEditingFinished()
{
    const T previousValue = m_value;
    setValue(QVariant::fromValue(m_valueEntry->value()));
    if (previousValue != m_value)
    {
        emit valueUpdated(QVariant::fromValue(m_value));
    }
}

// Explicit instantiations
template class ValueLineEdit<int32_t>;
template class ValueLineEdit<uint32_t>;
template class ValueLineEdit<float>;
template class ValueLineEdit<double>;
template class SliderGroup<int32_t>;
template class SliderGroup<uint32_t>;
template class SliderGroup<float>;
template class SliderGroup<double>;

} // namespace TINKERUSD_NS