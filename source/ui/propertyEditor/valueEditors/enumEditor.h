#pragma once

#include "abstractPropertyEditor.h"
#include "enumWidget.h"

namespace TINKERUSD_NS
{

/**
 * @class EnumEditor
 * @brief Custom editor that uses a PropertyComboBoxWidget to display and edit enum values.
 *
 */
class EnumEditor : public AbstractPropertyEditor
{
public:
    EnumEditor(const QString& name, const EnumData& enumData, const QString& tooltip = QString());

    virtual ~EnumEditor() = default;

    // convert QVariant to Pixar's VtValue
    PXR_NS::VtValue toVtValue(const QVariant& value) const override;

    // convert Pixar's VtValue back to QVariant
    QVariant fromVtValue(const PXR_NS::VtValue& value) const override;

    // create the editor widget for the property
    QWidget* createEditor(QWidget* parent) const override;

    // set the data for the editor widget
    void setEditorData(QWidget* editor, const QVariant& data) const override;

    // get the current data from the editor widget
    QVariant editorData(QWidget* editor) const override;

    bool paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value) const override;

private:
    QStringList m_options;
};

} // namespace TINKERUSD_NS