#pragma once

#include "modelview/usdAttributeWrapper.h"

#include <QtCore/QVariant>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionViewItem>
#include <QtWidgets/QWidget>

namespace TINKERUSD_NS
{

/**
 * @class AbstractPropertyEditor
 * @brief Base class for all property editors within the Property Editor framework.
 *
 */
class AbstractPropertyEditor
{
public:
    AbstractPropertyEditor(
        const QString&  name,
        const QVariant& currentValue,
        const QString&  tooltip = QString());

    ~AbstractPropertyEditor() = default;

    // converts a QVariant value to a PXR_NS::VtValue.
    virtual PXR_NS::VtValue toVtValue(const QVariant& value) const = 0;

    // converts a PXR_NS::VtValue to a QVariant.
    virtual QVariant fromVtValue(const PXR_NS::VtValue& value) const = 0;

    // creates the editor widget for the property.
    virtual QWidget* createEditor(QWidget* parent) const = 0;

    // sets the data in the editor widget.
    virtual void setEditorData(QWidget* editor, const QVariant& data) const = 0;

    // retrieves the data from the editor widget.
    virtual QVariant editorData(QWidget* editor) const = 0;

    // paints the editor's display value within a view.
    virtual bool paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value) const;

    // returns the name of the property editor.
    QString name() const;

    // sets the name of the property editor.
    void setName(const QString& name);

    // returns the current value of the property.
    QVariant currentValue() const;

    // sets the current value of the property.
    void setCurrentValue(const QVariant& value);

    // returns the default value of the property.
    QVariant defaultValue() const;

    // returns if the current value is the default value.
    bool isDefault() const;

    // returns the tooltip associated with the property editor.
    QString tooltip() const;

    // returns the UsdAttributeWrapper associated with this property editor.
    UsdAttributeWrapper* usdAttributeWrapper() const;

    // sets the UsdAttributeWrapper for this property editor.
    void setAttributeWrapper(UsdAttributeWrapper::Ptr usdAttrWrapper);

private:
    QString                  m_name;
    QString                  m_tooltip;
    QVariant                 m_currentValue;
    QVariant                 m_defaultValue;
    UsdAttributeWrapper::Ptr m_usdAttributeWrapper;
};

} // namespace TINKERUSD_NS