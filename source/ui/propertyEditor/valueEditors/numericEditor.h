#pragma once

#ifndef TINKERUSD_NUMERIC_EDITOR_H
#define TINKERUSD_NUMERIC_EDITOR_H

#include "abstractPropertyEditor.h"
#include "customWidgets/numericWidgets.h"

#include <QVBoxLayout>
#include <QtCore/QPair>

namespace TINKERUSD_NS
{

class AbstractSliderGroupWidget : public QWidget
{
    Q_OBJECT
public:
    AbstractSliderGroupWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }
    virtual ~AbstractSliderGroupWidget() = default;

    virtual QVariant getValue() const = 0;
    virtual void     setValue(const QVariant& value) = 0;

signals:
    void commitData();

protected slots:
    void onValueEntryChanged() { emit commitData(); }
    void onSliderChanged() { emit commitData(); }
};

template <typename T> struct NumericGroupSliderData
{
    QPair<T, T> m_range;
    T           m_defaultValue;

    NumericGroupSliderData()
        : m_defaultValue(T(0))
    {
    }
    NumericGroupSliderData(const QPair<T, T>& range, T defaultValue)
        : m_range(range)
        , m_defaultValue(defaultValue)
    {
    }
};

Q_DECLARE_METATYPE(TINKERUSD_NS::NumericGroupSliderData<int32_t>)
Q_DECLARE_METATYPE(TINKERUSD_NS::NumericGroupSliderData<uint32_t>)
Q_DECLARE_METATYPE(TINKERUSD_NS::NumericGroupSliderData<float>)
Q_DECLARE_METATYPE(TINKERUSD_NS::NumericGroupSliderData<double>)

template <typename T> class NumericSliderGroupWidget : public AbstractSliderGroupWidget
{
public:
    NumericSliderGroupWidget(QWidget* parent = nullptr)
        : AbstractSliderGroupWidget(parent)
        , m_sliderGroup(new SliderGroup<T>(parent))
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_sliderGroup);

        connect(
            m_sliderGroup, &SliderGroupBase::sliderMoved, this, &NumericSliderGroupWidget::onSliderChanged);
        connect(
            m_sliderGroup,
            &SliderGroupBase::valueUpdated,
            this,
            &NumericSliderGroupWidget::onValueEntryChanged);
    }

    QVariant getValue() const override { return QVariant::fromValue(m_sliderGroup->valueEntry()->value()); }

    void setValue(const QVariant& value) override { m_sliderGroup->setValue(value); }

    void setRange(const QPair<T, T>& range) { m_sliderGroup->setRange(range.first, range.second); }

private:
    SliderGroup<T>* m_sliderGroup;
};

template <typename T> class NumericEditor : public AbstractPropertyEditor
{
public:
    NumericEditor(
        const QString&                   name,
        const NumericGroupSliderData<T>& groupSliderData,
        const QString&                   tooltip = QString())
        : AbstractPropertyEditor(name, QVariant::fromValue(groupSliderData.m_defaultValue), tooltip)
        , m_range(groupSliderData.m_range)
    {
    }

    PXR_NS::VtValue toVtValue(const QVariant& value) const override
    {
        if (value.canConvert<T>())
            return PXR_NS::VtValue(value.value<T>());
        return PXR_NS::VtValue();
    }

    QVariant fromVtValue(const PXR_NS::VtValue& value) const override
    {
        if (value.IsHolding<T>())
            return QVariant(value.Get<T>());
        return QVariant();
    }

    QWidget* createEditor(QWidget* parent) const override
    {
        NumericSliderGroupWidget<T>* widget = new NumericSliderGroupWidget<T>(parent);
        widget->setRange(m_range);
        widget->setValue(currentValue());
        return widget;
    }

    void setEditorData(QWidget* editor, const QVariant& data) const override
    {
        if (auto* widget = dynamic_cast<AbstractSliderGroupWidget*>(editor))
            widget->setValue(data);
    }

    QVariant editorData(QWidget* editor) const override
    {
        if (auto* widget = dynamic_cast<AbstractSliderGroupWidget*>(editor))
            return widget->getValue();
        return QVariant();
    }

private:
    QPair<T, T> m_range;
};

using IntegerGroupSliderData = NumericGroupSliderData<int32_t>;
using UnsignedIntegerGroupSliderData = NumericGroupSliderData<uint32_t>;
using FloatGroupSliderData = NumericGroupSliderData<float>;
using DoubleGroupSliderData = NumericGroupSliderData<double>;

using IntegerEditor = NumericEditor<int32_t>;
using UnsignedIntegerEditor = NumericEditor<uint32_t>;
using FloatEditor = NumericEditor<float>;
using DoubleEditor = NumericEditor<double>;

} // namespace TINKERUSD_NS

#endif // TINKERUSD_NUMERIC_EDITOR_H