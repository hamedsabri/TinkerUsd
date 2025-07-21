#pragma once

#include <QtGui/QStandardItemModel>
#include <pxr/usd/usd/variantSets.h>

namespace TINKERUSD_NS
{

/**
 * @class PropertyModel
 * @brief A model representing properties of a USD Prim
 *
 */
class PropertyModel : public QStandardItemModel
{
    Q_OBJECT
public:
    PropertyModel(QObject* parent = nullptr);

    virtual ~PropertyModel() = default;

    // retrieves data stored under the given role for the item referred to by the index.
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // sets the role data for the item at index to value.
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    // returns the item flags for the given index.
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // loads the properties ( attributes, relationships ) from the associated USD Prim.
    void loadUsdProperties();

    // loads the attributes from the associated Variant Sets.
    void loadVariantSets();

    // returns the total count of properties, including those in groups.
    int32_t propertyCount() const;

    // resets the model, clearing all items and properties.
    void reset();

private:
    // finds or creates a group item with the given name.
    QStandardItem* findOrCreateGroupItem(const QString& groupName);

    // adds a new property
    void addProperty(const PXR_NS::UsdAttribute& usdAttr);

    // adds a property to an existing group;
    void addPropertyToGroup(const QString& groupName, const PXR_NS::UsdAttribute& usdAttr);

    // adds variant sets to an existing group;
    void addVariantSetsToGroup(const QString& groupName, const PXR_NS::UsdVariantSet& variantSet);

    // counts all properties recursively, including those in groups.
    int32_t countAllProperties(const QStandardItem* parentItem) const;
};

} //  namespace TINKERUSD_NS