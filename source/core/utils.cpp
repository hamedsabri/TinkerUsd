#include "utils.h"

#include "globalSelection.h"

namespace TINKERUSD_NS
{

PXR_NS::UsdPrim selectedPrim() 
{ 
	return GlobalSelection::instance().prim(); 
}

} // namespace TINKERUSD_NS