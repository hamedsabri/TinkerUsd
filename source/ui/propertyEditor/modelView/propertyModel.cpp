#include "propertymodel.h"

#include "propertyItem.h"
#include "valueEditors/factory.h"

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace TINKERUSD_NS
{

PropertyModel::PropertyModel(QObject* parent)
    : QStandardItemModel(parent)
{
    setHorizontalHeaderLabels({ "Name", "Value" });
}

void PropertyModel::setUsdPrim(const PXR_NS::UsdPrim& prim) { m_prim = prim; }

void PropertyModel::reset()
{
    clear();
    setHorizontalHeaderLabels({ "Name", "Value" });
}

PXR_NS::UsdPrim const& PropertyModel::getUsdPrim() const { return m_prim; }

QStandardItem* PropertyModel::findOrCreateGroupItem(const QString& groupName)
{
    QList<QStandardItem*> items = findItems(groupName);
    QStandardItem*        groupItem;

    // if the group doesn't exist, create it first
    if (items.isEmpty())
    {
        groupItem = new QStandardItem(groupName);
        invisibleRootItem()->appendRow(groupItem);
    }
    else
    {
        groupItem = items.first();
    }

    return groupItem;
}

void PropertyModel::loadUsdAttributes()
{
    for (const auto& attr : m_prim.GetAttributes())
    {
        addPropertyToGroup(QString::fromStdString(m_prim.GetTypeName().GetString()), attr);
    }
}

void PropertyModel::loadVariantSets()
{
    PXR_NS::UsdVariantSets variantSets = m_prim.GetVariantSets();
    for (const auto& variantSetName : variantSets.GetNames())
    {
        auto variantSet = variantSets.GetVariantSet(variantSetName);
        addVariantSet(variantSet);
    }
}

void PropertyModel::addVariantSet(const PXR_NS::UsdVariantSet& variantSet)
{
    QStandardItem* groupItem = findOrCreateGroupItem("Variants");

    VariantSetEditor* editor = TINKERUSD_NS::createVariantSetEditor(variantSet);
    if (editor)
    {
        PropertyItem* nameItem = new PropertyItem(editor->name(), QVariant(), editor);
        PropertyItem* valueItem = new PropertyItem(QString(), editor->currentValue(), editor);
        groupItem->appendRow({ nameItem, valueItem });
    }
}

void PropertyModel::addProperty(const PXR_NS::UsdAttribute& usdAttr)
{
    AbstractPropertyEditor* property = TINKERUSD_NS::createNewAttributeEditor(usdAttr);
    PropertyItem*           nameItem = new PropertyItem(property->name(), QVariant(), property);
    PropertyItem* valueItem = new PropertyItem(QString(), property->currentValue(), property);
    appendRow({ nameItem, valueItem });
}

void PropertyModel::addPropertyToGroup(
    const QString&              groupName,
    const PXR_NS::UsdAttribute& usdAttr)
{
    QStandardItem* groupItem = findOrCreateGroupItem(groupName);

    AbstractPropertyEditor* property = TINKERUSD_NS::createNewAttributeEditor(usdAttr);
    PropertyItem*           nameItem = new PropertyItem(property->name(), QVariant(), property);
    PropertyItem* valueItem = new PropertyItem(QString(), property->currentValue(), property);
    groupItem->appendRow({ nameItem, valueItem });
}

QVariant PropertyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    // align the text in the first column (property names) to the left.
    if (role == Qt::TextAlignmentRole && index.column() == 0)
    {
        return Qt::AlignLeft;
    }
    else if (role == Qt::ToolTipRole && index.column() == 0)
    {
        AbstractPropertyEditor* property = TINKERUSD_NS::Utils::getEditorFromIndex(index);
        return property ? property->tooltip() : QVariant();
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
            AbstractPropertyEditor* property = TINKERUSD_NS::Utils::getEditorFromIndex(index);
            return property ? property->name() : QStandardItemModel::data(index, role).toString();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        AbstractPropertyEditor* property = TINKERUSD_NS::Utils::getEditorFromIndex(index);

        if (property && !property->isDefault())
        {
            QColor mildGreen(144, 238, 144);
            return QBrush(mildGreen);
        }
    }

    return QStandardItemModel::data(index, role);
}

bool PropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    if (role == Qt::EditRole && index.column() == 1)
    {
        AbstractPropertyEditor* propertyEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);

        if (auto* variantSetEditor = dynamic_cast<VariantSetEditor*>(propertyEditor))
        {
            variantSetEditor->setCurrentValue(value);

            PXR_NS::UsdVariantSet variantSet = variantSetEditor->getVariantSet();
            if (variantSet.SetVariantSelection(value.toString().toStdString()))
            {
                const QModelIndex nameIndex = this->index(index.row(), 0, index.parent());
                emit dataChanged(nameIndex, nameIndex, { Qt::FontRole, Qt::ForegroundRole });
            }
        }
        else
        {
            // set editor current value
            propertyEditor->setCurrentValue(value);

            // set usd attribute value
            PXR_NS::VtValue vtValue = propertyEditor->toVtValue(value);
            if (propertyEditor->usdAttributeWrapper()->set(vtValue, PXR_NS::UsdTimeCode::Default()))
            {

                // send dataChanged signal to update the view
                const QModelIndex nameIndex = this->index(index.row(), 0, index.parent());
                emit dataChanged(nameIndex, nameIndex, { Qt::FontRole, Qt::ForegroundRole });
            }
        }

        return QStandardItemModel::setData(index, value, role);
    }
    return false;
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex& index) const
{
    // make the first column non-editable
    Qt::ItemFlags flags = QStandardItemModel::flags(index);
    if (index.column() == 0)
    {
        flags &= ~Qt::ItemIsEditable;
    }

    return flags;
}

int32_t PropertyModel::propertyCount() const { return countAllProperties(invisibleRootItem()); }

int32_t PropertyModel::countAllProperties(const QStandardItem* parentItem) const
{
    int count = 0;
    for (int row = 0; row < parentItem->rowCount(); ++row)
    {
        const QStandardItem* item = parentItem->child(row);
        if (item->rowCount() > 0)
        {
            // item is a group, count its children recursively
            count += countAllProperties(item);
        }
        else
        {
            // item is a property
            count++;
        }
    }
    return count;
}

void PropertyModel::debugHierarchy() const
{
    QStandardItem* rootItem = invisibleRootItem();
    printItem(rootItem, 0, true);
}

void PropertyModel::printItem(QStandardItem* item, int indent, bool isRoot) const
{
    if (item == nullptr)
    {
        return;
    }

    // Get the property editor if available
    AbstractPropertyEditor* property
        = static_cast<AbstractPropertyEditor*>(item->data(Qt::UserRole).value<void*>());

    // Print the property name or the item's text
    const QString itemText = property ? property->name() : item->text();

    // Only print non-empty item texts
    if (!itemText.isEmpty())
    {
        const QString indentString(indent, ' ');
        if (isRoot)
        {
            qDebug() << "| " << itemText;
        }
        else
        {
            qDebug() << indentString + "| " << itemText;
        }
    }

    // Recursively print child items with increased indentation
    for (int i = 0; i < item->rowCount(); ++i)
    {
        printItem(item->child(i), indent + 4, false);
    }
}

} // namespace TINKERUSD_NS
