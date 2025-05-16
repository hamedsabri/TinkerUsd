#include "globalSelection.h"

using namespace PXR_NS;

namespace TINKERUSD_NS
{

GlobalSelection& GlobalSelection::instance()
{
    static GlobalSelection instance;
    return instance;
}

void GlobalSelection::setPrim(const UsdPrim& prim)
{
    if (prim == m_selectedPrim) {
        return;
    }

    m_selectedPrim = prim;

    // notify observers
    emit selectionChanged(m_selectedPrim);
}

UsdPrim GlobalSelection::prim() const 
{ 
    return m_selectedPrim; 
}

SdfPath GlobalSelection::path() const
{
    return m_selectedPrim ? m_selectedPrim.GetPath() : SdfPath();
}

void GlobalSelection::clearSelection()
{
    if (!m_selectedPrim.IsValid()) {
        return;
    }

    // invalidate the prim
    m_selectedPrim = UsdPrim();

    // notify observers
    emit selectionChanged(m_selectedPrim);
}

} // namespace TINKERUSD_NS
