#include "stringEditor.h"

#include <QFontMetrics>
#include <QtWidgets/QVBoxLayout>

namespace
{
// HS TODO: handle long chars properly
QString check(const QString& txt)
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

StringEditorWidget::StringEditorWidget(QWidget* parent)
    : QWidget(parent)
    , m_lineEdit(new QLineEdit(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    connect(m_lineEdit, &QLineEdit::editingFinished, this, [this]() { emit commitData(); });
}

QString StringEditorWidget::text() const { return m_lineEdit->text(); }

void StringEditorWidget::setText(const QString& text) { m_lineEdit->setText(text); }

void StringEditorWidget::setEditable(bool editable) { m_lineEdit->setReadOnly(!editable); }

StringEditor::StringEditor(const QString& name, const StringData& data, const QString& tooltip)
    : AbstractPropertyEditor(name, check(data.m_value), tooltip)
    , m_editable(data.m_editable)
{
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

QWidget* StringEditor::createEditor(QWidget* parent) const
{
    StringEditorWidget* widget = new StringEditorWidget(parent);
    widget->setToolTip(tooltip());
    widget->setEditable(m_editable);
    return widget;
}

void StringEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    StringEditorWidget* lineEdit = qobject_cast<StringEditorWidget*>(editor);
    if (lineEdit)
    {
        lineEdit->setText(data.toString());
    }
}

QVariant StringEditor::editorData(QWidget* editor) const
{
    StringEditorWidget* lineEdit = qobject_cast<StringEditorWidget*>(editor);
    if (lineEdit)
    {
        return lineEdit->text();
    }
    return QVariant();
}

} // namespace TINKERUSD_NS