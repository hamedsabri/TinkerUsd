#pragma once

#include "abstractPropertyEditor.h"

#include <concepts>
#include <pxr/base/gf/matrix2d.h>
#include <pxr/base/gf/matrix3d.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/array.h>
#include <pxr/base/vt/value.h>

namespace TINKERUSD_NS
{

template <typename T>
concept FundamentalType = std::is_fundamental_v<T>;

// registering VtArray types. add more types here as needed...
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
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec4f>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec4d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfMatrix2d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfMatrix3d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfMatrix4d>)
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
 * @class ArrayEditorWidget
 * @brief A template-based widget for displaying VtArray types.
 *
 */
template <typename T> class ArrayEditorWidget : public ArrayEditorWidgetBase
{
    using ParamType = std::conditional_t<FundamentalType<T>, T, const T&>;

public:
    ArrayEditorWidget(QWidget* parent, const PXR_NS::VtArray<T>& array, const QString& typeName);

    virtual ~ArrayEditorWidget() = default;

    // returns the array data as a QVariant
    QVariant arrayData() const override;

    // sets the array data from a QVariant
    void setArrayData(const QVariant& data) override;

    // formats a single element of the array into a string for display
    QString formatToString(ParamType value) const;

private:
    QString displayData(const PXR_NS::VtArray<T>& array) const;

    PXR_NS::VtArray<T> m_array;
    QLineEdit*         m_lineEdit;
    QString            m_typeDisplayName;
};

/**
 * @class ArrayEditorBase
 * @brief A base class for array editors in the property editor.
 *
 */
class ArrayEditorBase : public AbstractPropertyEditor
{
public:
    ArrayEditorBase(const QString& name, const QVariant& currentValue, const QString& tooltip)
        : AbstractPropertyEditor(name, currentValue, tooltip)
    {
    }
    virtual ~ArrayEditorBase() = default;

    // returns the raw array value as a string
    virtual QString getRawArrayValue() const = 0;
};

/**
 * @class ArrayEditor
 * @brief A template-based array editor for specific VtArray types.
 *
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

    // returns the raw array value as a string
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