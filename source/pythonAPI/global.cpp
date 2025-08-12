#include "global.h"

#include <TinkerUsd/core/utils.h>

namespace TINKERUSD_NS
{

PXR_NS::UsdStageRefPtr stage() { return selectedPrim().GetStage(); }

PXR_NS::SdfPath primSelPath() { return selectedPrimPath(); }

PXR_NS::UsdPrim primSel()  { return selectedPrim(); }

PXR_NS::SdfLayerHandle editTargetLayer()
{
	return stage()->GetEditTarget().GetLayer();
}

} // namespace TINKERUSD_NS
