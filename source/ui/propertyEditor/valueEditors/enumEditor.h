#pragma once

#include "abstractPropertyEditor.h"
#include "enumWidget.h"

namespace TINKERUSD_NS
{

class EnumEditor : public AbstractPropertyEditor
{
public:
    EnumEditor(const QString& name, const EnumData& enumData, const QString& tooltip = QString());

    PXR_NS::VtValue toVtValue(const QVariant& value) const override;
    QVariant        fromVtValue(const PXR_NS::VtValue& value) const override;

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

private:
    QStringList m_options;
};

} // namespace TINKERUSD_NS