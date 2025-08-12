#pragma once

#include "api.h"

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/usd/prim.h>

namespace TINKERUSD_NS
{

TINKERUSD_API_PUBLIC
PXR_NS::UsdStageRefPtr stage();

TINKERUSD_API_PUBLIC
PXR_NS::SdfPath primSelPath();

TINKERUSD_API_PUBLIC
PXR_NS::UsdPrim primSel();

TINKERUSD_API_PUBLIC
PXR_NS::SdfLayerHandle editTargetLayer();

} // namespace TINKERUSD_NS
