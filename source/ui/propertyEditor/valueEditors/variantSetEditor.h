#pragma once

#include "abstractPropertyEditor.h"
#include "enumWidget.h"

#include <pxr/usd/usd/variantSets.h>

namespace TINKERUSD_NS
{
class VariantSetEditor : public AbstractPropertyEditor
{
public:
    VariantSetEditor(
        const QString&               name,
        const EnumData&              enumData,
        const PXR_NS::UsdVariantSet& variantSet,
        const QString&               tooltip = QString());

    PXR_NS::VtValue toVtValue(const QVariant& value) const override;
    QVariant        fromVtValue(const PXR_NS::VtValue& value) const override;

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;
    bool     paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value)
        const override;

    PXR_NS::UsdVariantSet getVariantSet() const { return m_variantSet; }

private:
    QStringList           m_options;
    PXR_NS::UsdVariantSet m_variantSet;
};

} // namespace TINKERUSD_NS