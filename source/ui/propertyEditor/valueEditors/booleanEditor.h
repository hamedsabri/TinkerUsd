#pragma once

#include "abstractPropertyEditor.h"

#include <QtWidgets/QCheckBox>

namespace TINKERUSD_NS
{

class PropertyCheckBoxWidget : public QWidget
{
    Q_OBJECT
public:
    PropertyCheckBoxWidget(QWidget* parent = nullptr);
    bool isChecked() const;

public slots:
    void setChecked(bool c);

signals:
    void commitData();

private:
    QCheckBox* m_checkBox;
};

class BooleanEditor : public AbstractPropertyEditor
{
public:
    BooleanEditor(const QString& name, bool value, const QString& tooltip = QString());

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;
    bool     paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value)
        const override;

    PXR_NS::VtValue toVtValue(const QVariant& value) const override
    {
        return PXR_NS::VtValue(value.toBool());
    }

    QVariant fromVtValue(const PXR_NS::VtValue& value) const
    {
        if (value.IsHolding<bool>())
        {
            return QVariant(value.Get<bool>());
        }
        return QVariant();
    }
};

} // namespace TINKERUSD_NS