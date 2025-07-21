#include "outlinerModel.h"

#include <QString>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usd/stage.h>
using namespace pxr;

namespace TINKERUSD_NS
{

UsdOutlinerModel::UsdOutlinerModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

UsdOutlinerFilterProxyModel::UsdOutlinerFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);

    // we want to display all ancestors of matching items
    setRecursiveFilteringEnabled(true);
}

void UsdOutlinerFilterProxyModel::setFilterPattern(const QString& pattern)
{
    if (m_filterPattern != pattern)
    {
        m_filterPattern = pattern;
        invalidateFilter();
    }
}

bool UsdOutlinerFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_filterPattern.isEmpty())
    {
        return true;
    }

    const QAbstractItemModel* model = sourceModel();
    QModelIndex               nameIndex = model->index(sourceRow, 0, sourceParent);

    if (!nameIndex.isValid())
    {
        return false;
    }

    QString primName = model->data(nameIndex, Qt::DisplayRole).toString();

    return primName.contains(m_filterPattern, Qt::CaseInsensitive);
}

PXR_NS::UsdPrim UsdOutlinerFilterProxyModel::primFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return PXR_NS::UsdPrim();
    }

    // map proxy index to source index and get the prim
    QModelIndex sourceIndex = mapToSource(index);
    return static_cast<UsdOutlinerModel*>(sourceModel())->primFromIndex(sourceIndex);
}

void UsdOutlinerModel::setStage(const UsdStageRefPtr& stage)
{
    beginResetModel();
    m_stage = stage;
    m_rootItem = stage ? UsdOutlinerItem::create(stage->GetPseudoRoot()) : nullptr;
    endResetModel();
}

QModelIndex UsdOutlinerModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_rootItem || row < 0 || column < 0 || column >= 3)
    {
        return {};
    }

    UsdOutlinerItem::Ptr parentItem;

    if (!parent.isValid())
    {
        parentItem = m_rootItem;
    }
    else
    {
        auto* rawItem = static_cast<UsdOutlinerItem*>(parent.internalPointer());
        parentItem = rawItem ? rawItem->shared_from_this() : nullptr;
    }

    if (!parentItem)
    {
        return {};
    }

    UsdOutlinerItem::Ptr childItem = parentItem->child(row);
    if (!childItem)
    {
        return {};
    }

    return createIndex(row, column, childItem.get());
}

QModelIndex UsdOutlinerModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
    {
        return {};
    }

    auto* childItemRaw = static_cast<UsdOutlinerItem*>(child.internalPointer());
    if (!childItemRaw)
    {
        return {};
    }

    UsdOutlinerItem::Ptr childItem = childItemRaw->shared_from_this();
    UsdOutlinerItem::Ptr parentItem = childItem->parentItem();

    if (!parentItem || parentItem == m_rootItem)
    {
        return {};
    }

    return createIndex(parentItem->row(), 0, parentItem.get());
}

int UsdOutlinerModel::rowCount(const QModelIndex& parent) const
{
    if (!m_stage || !m_rootItem)
    {
        return 0;
    }

    UsdOutlinerItem::Ptr parentItem;

    if (!parent.isValid())
    {
        parentItem = m_rootItem;
    }
    else
    {
        auto* rawItem = static_cast<UsdOutlinerItem*>(parent.internalPointer());
        parentItem = rawItem ? rawItem->shared_from_this() : nullptr;
    }

    return parentItem ? parentItem->childCount() : 0;
}

int UsdOutlinerModel::columnCount(const QModelIndex&) const { return 3; }

QVariant UsdOutlinerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
    {
        return {};
    }

    auto* itemRaw = static_cast<UsdOutlinerItem*>(index.internalPointer());
    if (!itemRaw)
    {
        return {};
    }

    UsdOutlinerItem::Ptr item = itemRaw->shared_from_this();
    UsdPrim              prim = item->prim();

    switch (index.column())
    {
    case 0: return QString::fromUtf8(prim.GetName().GetText());
    case 1: return QString::fromUtf8(prim.GetTypeName().GetText());
    case 2: {
        TfToken kind;
        if (UsdModelAPI(prim).GetKind(&kind))
            return QString::fromUtf8(kind.GetText());
        return {};
    }
    default: return {};
    }
}

QVariant UsdOutlinerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return {};
    }

    switch (section)
    {
    case 0: return "Name";
    case 1: return "Type";
    case 2: return "Kind";
    default: return {};
    }
}

UsdPrim UsdOutlinerModel::primFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return UsdPrim();
    }

    auto* itemRaw = static_cast<UsdOutlinerItem*>(index.internalPointer());
    if (!itemRaw)
    {
        return UsdPrim();
    }

    UsdOutlinerItem::Ptr item = itemRaw->shared_from_this();
    return item ? item->prim() : UsdPrim();
}

QModelIndex UsdOutlinerModel::indexFromPrim(const UsdPrim& targetPrim) const
{
    std::function<QModelIndex(UsdOutlinerItem::Ptr)> findIndex;
    findIndex = [&](UsdOutlinerItem::Ptr item) -> QModelIndex {
        if (!item)
        {
            return {};
        }

        if (item->prim() == targetPrim)
        {
            return createIndex(item->row(), 0, item.get());
        }

        item->fetchChildrenIfNeeded();
        for (int i = 0; i < item->childCount(); ++i)
        {
            auto index = findIndex(item->child(i));
            if (index.isValid())
            {
                return index;
            }
        }
        return {};
    };

    return findIndex(m_rootItem);
}

} // namespace TINKERUSD_NS