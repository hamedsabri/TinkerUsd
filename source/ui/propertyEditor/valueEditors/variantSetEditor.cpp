#include "variantSetEditor.h"

#include <QtCore/QSignalBlocker>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOptionComboBox>

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

PXR_NS::VtValue VariantSetEditor::toVtValue(const QVariant& value) const
{
    return PXR_NS::VtValue();
}

QVariant VariantSetEditor::fromVtValue(const PXR_NS::VtValue& value) const { return QVariant(); }

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

bool VariantSetEditor::paint(
    QPainter*                   painter,
    const QStyleOptionViewItem& option,
    const QVariant&             value) const
{
    QStyleOptionComboBox comboBoxOption;
    comboBoxOption.rect = option.rect.adjusted(0, 100, 0, -100); // Adjust to match combo box size
    comboBoxOption.currentText = value.toString();
    comboBoxOption.state = option.state | QStyle::State_Enabled;

    // Draw combo box appearance instead of text
    QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &comboBoxOption, painter);
    QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboBoxOption, painter);
    return true;
}

} // namespace TINKERUSD_NS