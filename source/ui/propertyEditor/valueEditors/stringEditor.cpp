#include "stringEditor.h"

#include <QtWidgets/QVBoxLayout>

namespace
{
// HS TODO: handle long chars properly
QString checkLength(const QString& txt)
{
    if (txt.length() <= 200)
    {
        return txt.simplified();
    }
    return QString("Long String is not supported yet!");
}
} // namespace

namespace TINKERUSD_NS
{

PropertyStringWidget::PropertyStringWidget(QWidget* parent)
    : QWidget(parent)
    , m_lineEdit(new QLineEdit(parent))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // send commitData signal whenever the user finishes editing the text in the line edit
    connect(m_lineEdit, &QLineEdit::editingFinished, this, [=]() { Q_EMIT commitData(); });
}

QString PropertyStringWidget::text() const { return m_lineEdit->text(); }

void PropertyStringWidget::setText(const QString& text) { m_lineEdit->setText(text); }

void PropertyStringWidget::setEditable(bool editable) { m_lineEdit->setReadOnly(!editable); }

StringEditor::StringEditor(const QString& name, const StringData& value, const QString& tooltip)
    : AbstractPropertyEditor(name, checkLength(value.m_value), tooltip)
    , m_editable(value.m_editable)
{
}

QWidget* StringEditor::createEditor(QWidget* parent) const
{
    PropertyStringWidget* propertyStringWidget = new PropertyStringWidget(parent);
    propertyStringWidget->setToolTip(tooltip());
    propertyStringWidget->setEditable(m_editable);
    return propertyStringWidget;
}

PXR_NS::VtValue StringEditor::toVtValue(const QVariant& value) const
{
    return PXR_NS::VtValue(PXR_NS::TfStringTrim(value.toString().toStdString()));
}

QVariant StringEditor::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<std::string>())
    {
        return QVariant(QString::fromStdString(value.Get<std::string>()));
    }
    return QVariant();
}

void StringEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    PropertyStringWidget* propertyStringWidget = qobject_cast<PropertyStringWidget*>(editor);
    if (propertyStringWidget)
    {
        propertyStringWidget->setText(data.toString());
    }
}

QVariant StringEditor::editorData(QWidget* editor) const
{
    PropertyStringWidget* propertyStringWidget = qobject_cast<PropertyStringWidget*>(editor);
    if (propertyStringWidget)
    {
        return propertyStringWidget->text();
    }
    return QVariant();
}

} // namespace TINKERUSD_NS