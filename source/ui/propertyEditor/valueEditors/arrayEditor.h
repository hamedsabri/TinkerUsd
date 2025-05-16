#pragma once

#include "abstractPropertyEditor.h"

#include <concepts>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/array.h>
#include <pxr/base/vt/value.h>

namespace TINKERUSD_NS
{

template <typename T>
concept FundamentalType = std::is_fundamental_v<T>;

// add more types here as needed ...
Q_DECLARE_METATYPE(PXR_NS::VtArray<bool>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<int32_t>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<uint32_t>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<float>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<double>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<std::string>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec2f>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec2d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec3f>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec3d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::TfToken>)

/**
 * @class ArrayEditorWidgetBase
 * @brief A base class for ArrayEditorWidget that provides common functionality such as signals and
 * slots.
 *
 * This class cannot directly mix Q_OBJECT with templates, hence it serves as a common
 * interface for derived template-based widget.
 */
class ArrayEditorWidgetBase : public QWidget
{
    Q_OBJECT
public:
    ArrayEditorWidgetBase(QWidget* parent)
        : QWidget(parent)
    {
    }
    virtual ~ArrayEditorWidgetBase() = default;

    virtual QVariant arrayData() const = 0;
    virtual void     setArrayData(const QVariant& data) = 0;

signals:
    // signal emitted when the data should be committed.
    void commitData();
};

/**
 * @struct ArrayEditorWidget
 * @brief A template-based widget for displaying vt arays.
 */
template <typename T> class ArrayEditorWidget : public ArrayEditorWidgetBase
{
    using ParamType = std::conditional_t<FundamentalType<T>, T, const T&>;

public:
    ArrayEditorWidget(QWidget* parent, const PXR_NS::VtArray<T>& array, const QString& typeName);

    virtual ~ArrayEditorWidget() = default;

    QVariant arrayData() const override;
    void     setArrayData(const QVariant& data) override;
    QString  formatToString(ParamType value) const;

private:
    QString displayData(const PXR_NS::VtArray<T>& array) const;

    PXR_NS::VtArray<T> m_array;
    QLineEdit*         m_lineEdit;
    QString            m_typeDisplayName;
};

/**
 * @struct ArrayEditorBase
 * @brief
 */
class ArrayEditorBase : public AbstractPropertyEditor
{
public:
    ArrayEditorBase(const QString& name, const QVariant& currentValue, const QString& tooltip)
        : AbstractPropertyEditor(name, currentValue, tooltip)
    {
    }
    virtual ~ArrayEditorBase() = default;

    virtual QString getRawArrayValue() const = 0;
};

/**
 * @struct ArrayEditor
 * @brief
 */
template <typename T> class ArrayEditor : public ArrayEditorBase
{
public:
    ArrayEditor(
        const QString&            name,
        const PXR_NS::VtArray<T>& value,
        const QString&            tooltip,
        const QString&            typeName);

    virtual ~ArrayEditor() = default;

    // returns the raw array data
    QString getRawArrayValue() const override;

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
    PXR_NS::VtArray<T>            m_vtArray;
    QString                       m_typeDisplayName;
    mutable ArrayEditorWidget<T>* m_arrayEditorWidget;
};

} // namespace TINKERUSD_NS