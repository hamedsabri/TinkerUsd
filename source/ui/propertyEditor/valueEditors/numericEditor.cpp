#include "numericEditor.h"

#include "customWidgets/valueEntryLineEdit.h"

#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

template<typename T>
NumericSliderGroupWidget<T>::NumericSliderGroupWidget(QWidget *parent)  
    : AbstractSliderGroupWidget(parent)
    , m_sliderGroup(new SliderGroupType(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_sliderGroup);

    connect(m_sliderGroup->slider(), 
            &AttributeEditorSlider::sliderMoved, 
            this, 
            &AbstractSliderGroupWidget::onSliderChanged);

    if constexpr (std::disjunction_v<std::is_same<T, int32_t>, std::is_same<T, uint32_t>>) {
        connect(m_sliderGroup->valueEntry(), 
                &IntValueLineEdit::valueChanged, 
                this, 
                &AbstractSliderGroupWidget::onValueEntryChanged);
    } else {
        connect(m_sliderGroup->valueEntry(), 
                &DoubleValueLineEdit::valueChanged, 
                this, 
                &AbstractSliderGroupWidget::onValueEntryChanged);
    }
}

template<typename T>
QVariant NumericSliderGroupWidget<T>::getValue() const
{
    return m_sliderGroup->valueEntry()->value();
}

template<typename T>
void NumericSliderGroupWidget<T>::setValue(const QVariant& value)
{
    m_sliderGroup->setValue(value.value<T>());
}

template<typename T>
void NumericSliderGroupWidget<T>::setRange(const QPair<T, T>& range)
{
    m_sliderGroup->setRange(range.first, range.second);
}

template<typename T>
NumericEditor<T>::NumericEditor(const QString &name, 
                                const NumericGroupSliderData<T>& groupSliderData, 
                                const QString &tooltip)
    : AbstractPropertyEditor(name, QVariant::fromValue(groupSliderData.m_defaultValue), tooltip)
    , m_range(groupSliderData.m_range)
{
}

template<typename T>
PXR_NS::VtValue NumericEditor<T>::toVtValue(const QVariant& value) const
{
    if (value.canConvert<T>()) {
        return PXR_NS::VtValue(value.value<T>());
    }
    return PXR_NS::VtValue();
}

template<typename T>
QVariant NumericEditor<T>::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<T>()) {
        return QVariant(value.Get<T>());
    }
    return QVariant();
}

template<typename T>
QWidget* NumericEditor<T>::createEditor(QWidget* parent) const
{
    NumericSliderGroupWidget<T>* numericWidget = new NumericSliderGroupWidget<T>(parent);
    numericWidget->setRange(m_range);
    numericWidget->setValue(currentValue().value<T>());
    return numericWidget;
}

template<typename T>
void NumericEditor<T>::setEditorData(QWidget* editor, const QVariant& data) const
{
    AbstractSliderGroupWidget* abstractSliderGroupWidget = dynamic_cast<AbstractSliderGroupWidget*>(editor);
    if (abstractSliderGroupWidget){
        abstractSliderGroupWidget->setValue(data);
    }
}

template<typename T>
QVariant NumericEditor<T>::editorData(QWidget* editor) const
{
    AbstractSliderGroupWidget* abstractSliderGroupWidget = dynamic_cast<AbstractSliderGroupWidget*>(editor);
    if (abstractSliderGroupWidget) {
        return abstractSliderGroupWidget->getValue();
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