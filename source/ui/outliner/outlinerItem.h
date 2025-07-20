#pragma once

#include "core/utils.h"

#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>

#include <vector>
#include <memory>


namespace TINKERUSD_NS
{

class UsdOutlinerItem : public std::enable_shared_from_this<UsdOutlinerItem>
{
public:
    using Ptr = std::shared_ptr<UsdOutlinerItem>;
    using WeakPtr = std::weak_ptr<UsdOutlinerItem>;

    static Ptr create(const PXR_NS::UsdPrim& prim, Ptr parent = nullptr);

    DISALLOW_COPY_MOVE_ASSIGNMENT(UsdOutlinerItem);

    Ptr child(int row);
    int childCount();
    int row() const;

    Ptr parentItem();
    PXR_NS::UsdPrim prim() const;

    void fetchChildrenIfNeeded();

private:
    UsdOutlinerItem(const PXR_NS::UsdPrim& prim, Ptr parent);

private:
    PXR_NS::UsdPrim     m_prim;
    WeakPtr          m_parentItem;
    std::vector<Ptr> m_childItems;
    bool             m_childrenFetched;
};

} // namespace TINKERUSD_NS