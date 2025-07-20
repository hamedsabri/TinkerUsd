#include "booleanEditor.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOptionButton>
#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

PropertyCheckBoxWidget::PropertyCheckBoxWidget(QWidget* parent)
    : QWidget(parent)
    , m_checkBox(new QCheckBox(parent))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_checkBox);
    setLayout(layout);

    connect(m_checkBox, &QCheckBox::toggled, [this](bool){
        Q_EMIT commitData();
    });

    setFocusProxy(m_checkBox);
}

bool PropertyCheckBoxWidget::isChecked() const
{
    return m_checkBox->isChecked();
}

void PropertyCheckBoxWidget::setChecked(bool c)
{ 
    m_checkBox->setChecked(c);
}

BooleanEditor::BooleanEditor(const QString& name, bool value, const QString& tooltip)
    : AbstractPropertyEditor(name, value, tooltip)
{
}

QWidget* BooleanEditor::createEditor(QWidget* parent) const
{
    PropertyCheckBoxWidget* propertyCheckBoxWidget = new PropertyCheckBoxWidget(parent);
    propertyCheckBoxWidget->setToolTip(tooltip());
    propertyCheckBoxWidget->setChecked(currentValue().toBool());
    return propertyCheckBoxWidget;
}

PXR_NS::VtValue BooleanEditor::toVtValue(const QVariant& value) const
{
    return PXR_NS::VtValue(value.toBool());
}

QVariant BooleanEditor::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<bool>()) {
        return QVariant(value.Get<bool>());
    }
    return QVariant();
}

void BooleanEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    PropertyCheckBoxWidget* propertyCheckBoxWidget = qobject_cast<PropertyCheckBoxWidget*>(editor);
    if (propertyCheckBoxWidget) {
        propertyCheckBoxWidget->blockSignals(true);
        propertyCheckBoxWidget->setChecked(data.toBool());
        propertyCheckBoxWidget->blockSignals(false);
    }
}

QVariant BooleanEditor::editorData(QWidget* editor) const
{
    PropertyCheckBoxWidget* propertyCheckBoxWidget = qobject_cast<PropertyCheckBoxWidget*>(editor);
    if (propertyCheckBoxWidget) {
        return propertyCheckBoxWidget->isChecked();
    }
    return QVariant();
}

bool BooleanEditor::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value) const
{
    return true;
}

} // namespace TINKERUSD_NS