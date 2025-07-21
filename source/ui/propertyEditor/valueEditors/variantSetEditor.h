#pragma once

#include "abstractPropertyEditor.h"
#include "enumWidget.h"

#include <pxr/usd/usd/variantSets.h>

namespace TINKERUSD_NS
{

/**
 * @class VariantSetEditor
 * @brief Custom editor to display and edit variants.
 *
 */
class VariantSetEditor : public AbstractPropertyEditor
{
public:
    VariantSetEditor(
        const QString&               name,
        const EnumData&              enumData,
        const PXR_NS::UsdVariantSet& variantSet,
        const QString&               tooltip = QString());

    virtual ~VariantSetEditor() = default;

    // get the variant set.
    PXR_NS::UsdVariantSet getVariantSet() const;

    // author a variant selection for given variantSet
    bool setVariantSelection(const QVariant& variantName);

    // convert QVariant to Pixar's VtValue
    PXR_NS::VtValue toVtValue(const QVariant& value) const override;

    // convert Pixar's VtValue back to QVariant
    QVariant fromVtValue(const PXR_NS::VtValue& value) const override;

    // create the editor widget for the property
    QWidget* createEditor(QWidget* parent) const override;

    // set the data for the editor widget
    void setEditorData(QWidget* editor, const QVariant& data) const override;

    // get the current data from the editor widget
    QVariant editorData(QWidget* editor) const override;

private:
    QStringList           m_options;
    PXR_NS::UsdVariantSet m_variantSet;
};

} // namespace TINKERUSD_NS