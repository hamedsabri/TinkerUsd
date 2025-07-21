#pragma once

#include "abstractPropertyEditor.h"
#include "arrayEditor.h"
#include "booleanEditor.h"
#include "colorEditor.h"
#include "enumEditor.h"
#include "fileEditor.h"
#include "numericEditor.h"
#include "stringEditor.h"
#include "variantSetEditor.h"
#include "vector2dEditor.h"
#include "vector3dEditor.h"

namespace TINKERUSD_NS
{
// creates a new AbstractPropertyEditor based on the given UsdAttribute type.
AbstractPropertyEditor* createNewAttributeEditor(const PXR_NS::UsdAttribute& usdAttr);

// creates a new VariantSetEditor based on the given UsdVariantSet.
VariantSetEditor* createVariantSetEditor(const PXR_NS::UsdVariantSet& variantSet);

} // namespace TINKERUSD_NS