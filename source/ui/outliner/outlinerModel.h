#pragma once

#include "outlinerItem.h"

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include <pxr/usd/usd/prim.h>

#include <memory>

namespace TINKERUSD_NS
{
class UsdOutlinerModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    UsdOutlinerModel(QObject* parent = nullptr);
    virtual ~UsdOutlinerModel() = default;

    void setStage(const PXR_NS::UsdStageRefPtr& stage);

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int         rowCount(const QModelIndex& parent) const override;
    int         columnCount(const QModelIndex& parent) const override;
    QVariant    data(const QModelIndex& index, int role) const override;
    QVariant    headerData(int section, Qt::Orientation orientation, int role) const override;

    PXR_NS::UsdPrim primFromIndex(const QModelIndex& index) const;

    QModelIndex indexFromPrim(const PXR_NS::UsdPrim& prim) const;

private:
    PXR_NS::UsdStageRefPtr  m_stage;
    UsdOutlinerItem::Ptr m_rootItem;
};

class UsdOutlinerFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    UsdOutlinerFilterProxyModel(QObject* parent = nullptr);

    void setFilterPattern(const QString& pattern);

    PXR_NS::UsdPrim primFromIndex(const QModelIndex& index) const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString m_filterPattern;
};

} // namespace TINKERUSD_NS