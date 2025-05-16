#include "numericEditor.h"

#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

template <typename T>
NumericSliderGroupWidget<T>::NumericSliderGroupWidget(QWidget* parent)
    : SliderGroupWidgetBase(parent)
{
    if constexpr (std::disjunction_v<std::is_same<T, int32_t>, std::is_same<T, uint32_t>>)
    {
        m_sliderGroup = new IncrementalSliderGroup(false, this);
    }
    else
    {
        m_sliderGroup = new IncrementalSliderGroup(true, this);
    }
    connect(
        m_sliderGroup,
        &IncrementalSliderGroup::valueChanged,
        this,
        &SliderGroupWidgetBase::onValueEntryChanged);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_sliderGroup);
}

template <typename T> QVariant NumericSliderGroupWidget<T>::getValue() const
{
    return m_sliderGroup->value();
}

template <typename T> void NumericSliderGroupWidget<T>::setValue(const QVariant& value)
{
    m_sliderGroup->setValue(value.value<T>());
}

template <typename T> void NumericSliderGroupWidget<T>::setRange(const QPair<T, T>& range)
{
    m_sliderGroup->setRange(range.first, range.second);
}

template <typename T>
NumericEditor<T>::NumericEditor(
    const QString&                   name,
    const NumericGroupSliderData<T>& groupSliderData,
    const QString&                   tooltip)
    : AbstractPropertyEditor(name, QVariant::fromValue(groupSliderData.m_defaultValue), tooltip)
    , m_range(groupSliderData.m_range)
{
}

template <typename T> PXR_NS::VtValue NumericEditor<T>::toVtValue(const QVariant& value) const
{
    if constexpr (std::is_same_v<T, int32_t>)
    {
        return PXR_NS::VtValue(value.toInt());
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        return PXR_NS::VtValue(value.toUInt());
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return PXR_NS::VtValue(value.toFloat());
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return PXR_NS::VtValue(value.toDouble());
    }
    else
    {
        return PXR_NS::VtValue();
    }
}

template <typename T> QVariant NumericEditor<T>::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<int32_t>())
    {
        return QVariant(value.Get<int32_t>());
    }
    else if (value.IsHolding<uint32_t>())
    {
        return QVariant(value.Get<uint32_t>());
    }
    else if (value.IsHolding<float>())
    {
        return QVariant(value.Get<float>());
    }
    else if (value.IsHolding<double>())
    {
        return QVariant(value.Get<double>());
    }

    return QVariant();
}

template <typename T> QWidget* NumericEditor<T>::createEditor(QWidget* parent) const
{
    NumericSliderGroupWidget<T>* editor = new NumericSliderGroupWidget<T>(parent);
    editor->setRange(m_range);
    editor->setValue(currentValue().value<T>());
    return editor;
}

template <typename T>
void NumericEditor<T>::setEditorData(QWidget* editor, const QVariant& data) const
{
    SliderGroupWidgetBase* sliderGroup = dynamic_cast<SliderGroupWidgetBase*>(editor);
    if (sliderGroup)
    {
        sliderGroup->setValue(data);
    }
}

template <typename T> QVariant NumericEditor<T>::editorData(QWidget* editor) const
{
    SliderGroupWidgetBase* sliderGroup = dynamic_cast<SliderGroupWidgetBase*>(editor);
    if (sliderGroup)
    {
        return sliderGroup->getValue();
    }
    return QVariant();
}

// explicit instantiation
template class NumericSliderGroupWidget<int32_t>;
template class NumericSliderGroupWidget<uint32_t>;
template class NumericSliderGroupWidget<float>;
template class NumericSliderGroupWidget<double>;
template class NumericEditor<int32_t>;
template class NumericEditor<uint32_t>;
template class NumericEditor<float>;
template class NumericEditor<double>;

} // namespace TINKERUSD_NS