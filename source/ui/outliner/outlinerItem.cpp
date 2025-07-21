#include "outlinerItem.h"

using namespace pxr;

namespace TINKERUSD_NS
{

UsdOutlinerItem::Ptr UsdOutlinerItem::create(const UsdPrim& prim, Ptr parent)
{
    return std::shared_ptr<UsdOutlinerItem>(new UsdOutlinerItem(prim, parent));
}

UsdOutlinerItem::UsdOutlinerItem(const UsdPrim& prim, Ptr parent)
    : m_prim(prim)
    , m_parentItem(parent)
    , m_childrenFetched(false)
{
}

void UsdOutlinerItem::fetchChildrenIfNeeded()
{
    if (m_childrenFetched)
    {
        return;
    }

    m_childrenFetched = true;

    m_childItems.clear();

    // get all child prims that match the default predicate
    for (const auto& childPrim : m_prim.GetFilteredChildren(UsdPrimDefaultPredicate))
    {
        m_childItems.push_back(create(childPrim, shared_from_this()));
    }
}

UsdOutlinerItem::Ptr UsdOutlinerItem::child(int row)
{
    fetchChildrenIfNeeded();

    if (row < 0 || row >= static_cast<int>(m_childItems.size()))
    {
        return nullptr;
    }

    return m_childItems[row];
}

int UsdOutlinerItem::childCount()
{
    fetchChildrenIfNeeded();
    return static_cast<int>(m_childItems.size());
}

int UsdOutlinerItem::row() const
{
    if (auto parent = m_parentItem.lock())
    {
        const auto& siblings = parent->m_childItems;
        for (size_t i = 0; i < siblings.size(); ++i)
        {
            if (siblings[i].get() == this)
            {
                return static_cast<int>(i);
            }
        }
    }
    return 0;
}

UsdOutlinerItem::Ptr UsdOutlinerItem::parentItem() { return m_parentItem.lock(); }

PXR_NS::UsdPrim UsdOutlinerItem::prim() const { return m_prim; }

} // namespace TINKERUSD_NS