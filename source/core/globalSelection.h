#pragma once

#include <QObject>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>

namespace TINKERUSD_NS
{

class GlobalSelection : public QObject
{
    Q_OBJECT
public:
    static GlobalSelection& instance();

    void setPrim(const PXR_NS::UsdPrim& prim);
    void clearSelection();

    PXR_NS::UsdPrim prim() const;
    PXR_NS::SdfPath path() const;

signals:
    void selectionChanged(const PXR_NS::UsdPrim& selectedPrim);

private:
    GlobalSelection() = default;

    PXR_NS::UsdPrim m_selectedPrim;
};

} // namespace TINKERUSD_NS
