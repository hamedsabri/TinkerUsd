#pragma once

#include "abstractPropertyEditor.h"

#include <QtWidgets/QCheckBox>

namespace TINKERUSD_NS
{

/**
 * @class PropertyCheckBoxWidget
 * @brief A custom widget that contains a QCheckBox. It provides a slot to set 
 * the checked state and emits a commitData signal when the checked state is changed.
 */
class PropertyCheckBoxWidget : public QWidget
{
    Q_OBJECT
public:
    PropertyCheckBoxWidget(QWidget* parent = nullptr);
    virtual ~PropertyCheckBoxWidget() = default;

    bool isChecked() const;

public slots:
    void setChecked(bool c);

signals:
    // signal emitted when the data needs to be committed.
    void commitData();

private:
    QCheckBox* m_checkBox;
};

/**
 * @class BooleanEditor
 * @brief Custom editor that uses a PropertyCheckBoxWidget to display and edit boolean values.
 *
 */
class BooleanEditor : public AbstractPropertyEditor
{
public:
    BooleanEditor(const QString& name, 
                  bool value, 
                  const QString& tooltip = QString());

    virtual ~BooleanEditor() = default;

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

    bool paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value) const override;
};

} // namespace TINKERUSD_NS