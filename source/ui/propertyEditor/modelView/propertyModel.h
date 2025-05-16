#pragma once

#include "common/utils.h"

#include <QtGui/QStandardItemModel>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/variantSets.h>

namespace TINKERUSD_NS
{

class PropertyModel : public QStandardItemModel
{
    Q_OBJECT

public:
    PropertyModel(QObject* parent = nullptr);
    virtual ~PropertyModel() = default;

    void setUsdPrim(const PXR_NS::UsdPrim& prim);

    PXR_NS::UsdPrim const& getUsdPrim() const;

    void loadUsdAttributes();
    void loadVariantSets();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int32_t propertyCount() const;
    void    debugHierarchy() const;

    void reset();

private:
    QStandardItem* findOrCreateGroupItem(const QString& groupName);

    void addProperty(const PXR_NS::UsdAttribute& usdAttr);
    void addPropertyToGroup(const QString& groupName, const PXR_NS::UsdAttribute& usdAttr);
    void addVariantSet(const PXR_NS::UsdVariantSet& variantSet);

    int32_t countAllProperties(const QStandardItem* parentItem) const;
    void    printItem(QStandardItem* item, int indent, bool isRoot) const;

private:
    PXR_NS::UsdPrim m_prim;
};

} //  namespace TINKERUSD_NS
