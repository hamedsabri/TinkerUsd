#include "enumEditor.h"

#include <QtCore/QSignalBlocker>

namespace TINKERUSD_NS
{

EnumEditor::EnumEditor(const QString& name, 
                       const EnumData& enumData, 
                       const QString& tooltip)
    : AbstractPropertyEditor(name, enumData.currentText, tooltip)
    , m_options(enumData.options)
{
}

QWidget* EnumEditor::createEditor(QWidget* parent) const
{
    PropertyComboBoxWidget* comboBoxWidget = new PropertyComboBoxWidget(parent);
    comboBoxWidget->comboBox()->addItems(m_options);
    comboBoxWidget->comboBox()->setCurrentIndex(currentValue().toInt());
    comboBoxWidget->setToolTip(tooltip());

    return comboBoxWidget;
}

PXR_NS::VtValue EnumEditor::toVtValue(const QVariant& value) const
{
    return PXR_NS::VtValue(PXR_NS::TfToken(value.toString().toStdString()));
}

QVariant EnumEditor::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<PXR_NS::TfToken>()) {
        PXR_NS::TfToken token = value.Get<PXR_NS::TfToken>();
        QString strValue = QString::fromStdString(token.GetString());
        int index = m_options.indexOf(strValue);
        if (index != -1) {
            return QVariant(strValue);
        }
    }
    return QVariant();
}

void EnumEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    PropertyComboBoxWidget* comboBoxWidget = qobject_cast<PropertyComboBoxWidget*>(editor);
    if (comboBoxWidget) {
        QSignalBlocker blocker(comboBoxWidget);
        comboBoxWidget->comboBox()->setCurrentText(data.toString());
    }
}

QVariant EnumEditor::editorData(QWidget* editor) const
{
    PropertyComboBoxWidget* comboBoxWidget = qobject_cast<PropertyComboBoxWidget*>(editor);
    if (comboBoxWidget) {
        return comboBoxWidget->comboBox()->currentText();
    }
    return QVariant();
}

bool EnumEditor::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value) const
{
    return true;
}

} // namespace TINKERUSD_NS