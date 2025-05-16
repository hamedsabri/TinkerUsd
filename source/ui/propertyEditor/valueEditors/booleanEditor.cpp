#include "booleanEditor.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOptionButton>
#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

PropertyCheckBoxWidget::PropertyCheckBoxWidget(QWidget* parent)
    : QWidget(parent)
    , m_checkBox(new QCheckBox(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_checkBox);

    connect(m_checkBox, &QCheckBox::toggled, [this](bool) { emit commitData(); });

    setFocusProxy(m_checkBox);
}

bool PropertyCheckBoxWidget::isChecked() const { return m_checkBox->isChecked(); }

void PropertyCheckBoxWidget::setChecked(bool c) { m_checkBox->setChecked(c); }

BooleanEditor::BooleanEditor(const QString& name, bool value, const QString& tooltip)
    : AbstractPropertyEditor(name, value, tooltip)
{
}

QWidget* BooleanEditor::createEditor(QWidget* parent) const
{
    PropertyCheckBoxWidget* editor = new PropertyCheckBoxWidget(parent);
    editor->setToolTip(tooltip());
    editor->setChecked(currentValue().toBool());
    return editor;
}

void BooleanEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    PropertyCheckBoxWidget* checkBox = qobject_cast<PropertyCheckBoxWidget*>(editor);
    if (checkBox)
    {
        checkBox->blockSignals(true);
        checkBox->setChecked(data.toBool());
        checkBox->blockSignals(false);
    }
}

QVariant BooleanEditor::editorData(QWidget* editor) const
{
    PropertyCheckBoxWidget* checkBox = qobject_cast<PropertyCheckBoxWidget*>(editor);
    if (checkBox)
    {
        return checkBox->isChecked();
    }
    return QVariant();
}

bool BooleanEditor::paint(
    QPainter*                   painter,
    const QStyleOptionViewItem& option,
    const QVariant&             value) const
{
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    QStyleOptionButton checkBoxOption;
    checkBoxOption.state |= QStyle::State_Enabled;
    if (value.toBool())
    {
        checkBoxOption.state |= QStyle::State_On;
    }
    else
    {
        checkBoxOption.state |= QStyle::State_Off;
    }
    checkBoxOption.rect = option.rect;

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter);
    return true;
}

} // namespace TINKERUSD_NS