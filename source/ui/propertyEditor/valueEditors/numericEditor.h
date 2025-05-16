#pragma once

#include "abstractPropertyEditor.h"
#include "customWidgets/incrementalSlider.h"

#include <QtCore/QPair>

namespace TINKERUSD_NS
{

/**
 * @class SliderGroupWidgetBase
 * @brief A base class for slider group widgets that provides common functionality such as signals
 * and slots.
 *
 * This class cannot directly mix Q_OBJECT with templates, hence it serves as a common
 * interface for derived template-based slider widgets.
 */
class SliderGroupWidgetBase : public QWidget
{
    Q_OBJECT
public:
    // constructor
    SliderGroupWidgetBase(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }

    // destructor
    virtual ~SliderGroupWidgetBase() = default;

    // returns the current value.
    virtual QVariant getValue() const = 0;

    // set the current value.
    virtual void setValue(const QVariant& value) = 0;

signals:
    // signal emitted when the data should be committed.
    void commitData();

public slots:
    // slot called when the value entry changes, emitting commitData signal.
    void onValueEntryChanged() { emit commitData(); }

    // slot called when the slider value changes, emitting commitData signal.
    void onSliderChanged() { emit commitData(); }
};

/**
 * @struct NumericGroupSliderData
 * @brief A structure to hold data for numeric group sliders.
 *
 */
template <typename T> struct NumericGroupSliderData
{
    QPair<T, T> m_range;
    T           m_defaultValue;

    NumericGroupSliderData()
        : m_defaultValue(0)
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

/**
 * @class NumericSliderGroupWidget
 * @brief A template-based widget for a numeric slider group.
 *
 */
template <typename T> class NumericSliderGroupWidget : public SliderGroupWidgetBase
{
public:
    // constructor
    NumericSliderGroupWidget(QWidget* parent = nullptr);

    // destructor
    ~NumericSliderGroupWidget() = default;

    // Returns the current value of the slider as a QVariant.
    QVariant getValue() const override;

    // sets the current value.
    void setValue(const QVariant& value) override;

    // sets the range
    void setRange(const QPair<T, T>& range);

private:
    IncrementalSliderGroup* m_sliderGroup { nullptr };
};

/**
 * @class NumericEditor
 * @brief A template-based property editor for numeric values.
 *
 */
template <typename T> class NumericEditor : public AbstractPropertyEditor
{
public:
    // constructor
    NumericEditor(
        const QString&                   name,
        const NumericGroupSliderData<T>& groupSliderData,
        const QString&                   tooltip = QString());

    // destructor
    ~NumericEditor() = default;

    // converts a QVariant value to a VtValue.
    PXR_NS::VtValue toVtValue(const QVariant& value) const override;

    // converts a VtValue to a QVariant.
    QVariant fromVtValue(const PXR_NS::VtValue& value) const override;

    // creates an editor widget for the property.
    QWidget* createEditor(QWidget* parent) const override;

    // sets the data in the editor widget.
    void setEditorData(QWidget* editor, const QVariant& data) const override;

    // returns the data from the editor widget.
    QVariant editorData(QWidget* editor) const override;

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
