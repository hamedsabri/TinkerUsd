#include "utils.h"

#include "globalSelection.h"

#include <pxr/usd/usdGeom/bboxCache.h>
#include <pxr/base/gf/bbox3d.h>
#include <pxr/usdImaging/usdImaging/delegate.h>

namespace TINKERUSD_NS
{

PXR_NS::UsdPrim selectedPrim() 
{ 
	return GlobalSelection::instance().prim(); 
}

GfBBox3d stageBbox(const PXR_NS::UsdStageRefPtr& stage)
{
    UsdGeomBBoxCache bboxCache(UsdTimeCode::Default(), UsdGeomImageable::GetOrderedPurposeTokens());
    return bboxCache.ComputeWorldBound(stage->GetPseudoRoot());
}

GfBBox3d globalSelectionBbox(const PXR_NS::UsdStageRefPtr& stage)
{
    UsdGeomBBoxCache bboxCache(UsdTimeCode::Default(), UsdGeomImageable::GetOrderedPurposeTokens());
    auto selectedPrim = stage->GetPrimAtPath(GlobalSelection::instance().path());
    if( !selectedPrim.IsValid() ) {
        return GfBBox3d();
    }

    GfBBox3d worldbbox = bboxCache.ComputeWorldBound(selectedPrim);
    GfBBox3d bbox;
    return GfBBox3d::Combine(bbox, worldbbox);
}


} // namespace TINKERUSD_NS