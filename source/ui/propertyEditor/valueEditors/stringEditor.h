#pragma once

#include "abstractPropertyEditor.h"

#include <QtWidgets/QLineEdit>

namespace TINKERUSD_NS
{

// Widget to provide a string editor interface
class StringEditorWidget : public QWidget
{
    Q_OBJECT
public:
    StringEditorWidget(QWidget* parent = nullptr);
    virtual ~StringEditorWidget() = default;

    QString text() const;

public slots:
    void setText(const QString& text);
    void setEditable(bool editable);

signals:
    void commitData();

private:
    QLineEdit* m_lineEdit;
};

struct StringData
{
    QString m_value;
    bool    m_editable;

    StringData()
        : m_editable(false)
    {
    }
    StringData(const QString& value, bool editable)
        : m_value(value)
        , m_editable(editable)
    {
    }
};
Q_DECLARE_METATYPE(StringData)

// Editor class for string properties
class StringEditor : public AbstractPropertyEditor
{
public:
    StringEditor(const QString& name, const StringData& data, const QString& tooltip = QString());
    virtual ~StringEditor() = default;

    PXR_NS::VtValue toVtValue(const QVariant& value) const override;
    QVariant        fromVtValue(const PXR_NS::VtValue& value) const override;

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

private:
    bool m_editable; // Flag to control if the QLineEdit is editable
};

} // namespace TINKERUSD_NS
