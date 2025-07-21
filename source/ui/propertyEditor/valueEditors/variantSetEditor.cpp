#include "variantSetEditor.h"

#include <QtCore/QSignalBlocker>

namespace TINKERUSD_NS
{

VariantSetEditor::VariantSetEditor(
    const QString&               name,
    const EnumData&              enumData,
    const PXR_NS::UsdVariantSet& variantSet,
    const QString&               tooltip)
    : AbstractPropertyEditor(name, enumData.currentText, tooltip)
    , m_options(enumData.options)
    , m_variantSet(variantSet)
{
}

PXR_NS::UsdVariantSet VariantSetEditor::getVariantSet() const { return m_variantSet; }

bool VariantSetEditor::setVariantSelection(const QVariant& variantName)
{
    return m_variantSet.SetVariantSelection(variantName.toString().toStdString());
}

PXR_NS::VtValue VariantSetEditor::toVtValue(const QVariant& value) const
{
    // ignore, toVtValue doesn't apply to variants.
    return PXR_NS::VtValue();
}

QVariant VariantSetEditor::fromVtValue(const PXR_NS::VtValue& value) const
{
    // ignore, toVtValue doesn't apply to variants.
    return QVariant();
}

QWidget* VariantSetEditor::createEditor(QWidget* parent) const
{
    PropertyComboBoxWidget* comboBoxWidget = new PropertyComboBoxWidget(parent);
    comboBoxWidget->comboBox()->addItems(m_options);
    comboBoxWidget->comboBox()->setCurrentIndex(currentValue().toInt());
    comboBoxWidget->setToolTip(tooltip());

    return comboBoxWidget;
}

void VariantSetEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    PropertyComboBoxWidget* comboBoxWidget = qobject_cast<PropertyComboBoxWidget*>(editor);
    if (comboBoxWidget)
    {
        QSignalBlocker blocker(comboBoxWidget);
        comboBoxWidget->comboBox()->setCurrentText(data.toString());
    }
}

QVariant VariantSetEditor::editorData(QWidget* editor) const
{
    PropertyComboBoxWidget* comboBoxWidget = qobject_cast<PropertyComboBoxWidget*>(editor);
    if (comboBoxWidget)
    {
        return comboBoxWidget->comboBox()->currentText();
    }
    return QVariant();
}

} // namespace TINKERUSD_NS