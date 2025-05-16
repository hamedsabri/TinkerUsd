#include "global.h"

#include <TinkerUsd/core/utils.h>

namespace TINKERUSD_NS
{

PXR_NS::UsdStageRefPtr stage() 
{ 
    return selectedPrim().GetStage(); 
}

} // namespace TINKERUSD_NS
