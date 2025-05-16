#pragma once

#include "abstractPropertyEditor.h"
#include "arrayEditor.h"
#include "booleanEditor.h"
#include "colorEditor.h"
#include "enumEditor.h"
#include "fileEditor.h"
#include "modelview/usdAttributeWrapper.h"
#include "numericEditor.h"
#include "stringEditor.h"
#include "variantSetEditor.h"
#include "vector2dEditor.h"
#include "vector3dEditor.h"

namespace TINKERUSD_NS
{
AbstractPropertyEditor* createNewAttributeEditor(const PXR_NS::UsdAttribute& usdAttr);
VariantSetEditor*       createVariantSetEditor(const PXR_NS::UsdVariantSet& variantSet);

} // namespace TINKERUSD_NS