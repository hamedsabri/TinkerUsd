#pragma once

#include "abstractPropertyEditor.h"

#include <QtWidgets/QLineEdit>

namespace TINKERUSD_NS
{

struct StringData 
{
    QString m_value;
    bool m_editable;

    StringData() : m_editable(false) {}
    StringData(const QString& value, bool editable) : m_value(value), m_editable(editable) {}
};
Q_DECLARE_METATYPE(StringData)

/**
 * @class PropertyStringWidget
 * @brief A custom widget for editing string properties.
 *
 * This widget contains a QLineEdit for displaying and editing string values. 
 * It emits a commitData signal when the editing is finished.
 */
class PropertyStringWidget : public QWidget
{
    Q_OBJECT
public:
    PropertyStringWidget(QWidget* parent = nullptr);
    virtual ~PropertyStringWidget() = default;

    // gets the text from the line edit.
    QString text() const;

    // sets the text in the line edit.
    void setText(const QString& text);

    // sets the edit-ability.
    void setEditable(bool editable);

signals:
    // signal emitted when the data needs to be committed.
    void commitData();

private:
    QLineEdit* m_lineEdit;
};

/**
 * @class StringEditor
 * @brief A property editor for string values.
 *
 * This class provides an interface for creating and managing a string property editor. 
 * The actual editing is done via PropertyStringWidget.
 */
class StringEditor : public AbstractPropertyEditor
{
public:
    StringEditor(const QString& name, 
                 const StringData& value, 
                 const QString &tooltip = QString());

    virtual ~StringEditor() = default;

    // convert QVariant to Pixar's VtValue
    PXR_NS::VtValue toVtValue(const QVariant& value) const override;

    // convert Pixar's VtValue back to QVariant
    QVariant fromVtValue(const PXR_NS::VtValue& value) const override;

    // creates the editor widget.
    QWidget* createEditor(QWidget* parent) const override;

    // sets the data in the editor widget.
    void setEditorData(QWidget* editor, const QVariant& data) const override;

    // gets the data from the editor widget.
    QVariant editorData(QWidget* editor) const override;

private:
    bool m_editable;
};

} // namespace TINKERUSD_NS