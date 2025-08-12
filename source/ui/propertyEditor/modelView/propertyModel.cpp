#include "propertymodel.h"

#include "commands/usdUndoAttributeCommand.h"
#include "common/utils.h"
#include "core/globalSelection.h"
#include "propertyItem.h"
#include "valueEditors/factory.h"

namespace TINKERUSD_NS
{

PropertyModel::PropertyModel(QObject* parent)
    : QStandardItemModel(parent)
{
    setHorizontalHeaderLabels({ "Name", "Value" });
}

QVariant PropertyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::ToolTipRole && index.column() == 0)
    {
        auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
        return abstractPropEditor ? abstractPropEditor->tooltip() : QVariant();
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
            auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
            return abstractPropEditor ? abstractPropEditor->name()
                                      : QStandardItemModel::data(index, role).toString();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
        if (abstractPropEditor && !abstractPropEditor->isDefault())
        {
            QColor mildGreen(144, 238, 144);
            return QBrush(mildGreen);
        }
    }

    return QStandardItemModel::data(index, role);
}

// HS 2025:
// PropertyModel::setData() serves both user edits and internal undo/redo-triggered refresh.
//
// 1️ - When the user edits a property, a UsdUndoAttributeCommand is created and pushed to UndoManager::instance().undoStack().
// 2️-  Each UsdUndoAttributeCommand has a private UndoNotifier (QObject) which emits undoOrRedoPerformed()
//     when the command's undo() or redo() is executed.
// 3️-  PropertyModel connects this signal at the time of command creation that refreshes
//     only the affected row, reading the current value from the UsdAttributeWrapper.
// 4️-  To avoid recursive undo stack corruption, a guard (m_inUndoRedoRefresh) ensures that any setData() calls
//     originating from the refresh logic do NOT create new undo commands.
//
//  This approach provides a fine-grained UI updates (no full model reset required)

bool PropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    if (m_inUndoRedoRefresh) {
        return QStandardItemModel::setData(index, value, role);
    }

    if (role == Qt::EditRole && index.column() == 1)
    {

        AbstractPropertyEditor* abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
        // set editor current value
        abstractPropEditor->setCurrentValue(value);

        bool sendDataChangedSignal = false;

        if (VariantSetEditor* variantSetEditor = dynamic_cast<VariantSetEditor*>(abstractPropEditor))
        {
            if (variantSetEditor->setVariantSelection(value)) {
                sendDataChangedSignal = true;
            }
        }
        else
        {
            // set usd attribute value
            auto vtValue = abstractPropEditor->toVtValue(value);
            auto attributeWrapper = abstractPropEditor->usdAttributeWrapper();
            if (attributeWrapper) {
                auto attributeCommand
                    = new UsdUndoAttributeCommand(attributeWrapper, vtValue, PXR_NS::UsdTimeCode::Default());
                
                if (attributeCommand) {
                    sendDataChangedSignal = true;
                }

                attributeCommand->setRefreshCallback([this, index]() {
                    refreshRowFromUsd(index);
                });
            }
        }

        if (sendDataChangedSignal)
        {
            // send dataChanged signal to update the view
            const QModelIndex nameIndex = this->index(index.row(), 0, index.parent());
            emit  dataChanged(nameIndex, nameIndex, { Qt::FontRole, Qt::ForegroundRole });
        }

        return QStandardItemModel::setData(index, value, role);
    }

    return false;
}

void PropertyModel::refreshRowFromUsd(const QModelIndex& index)
{
    PropertyItem* item = static_cast<PropertyItem*>(itemFromIndex(index));
    if (!item){
        return;
    }

    AbstractPropertyEditor* propEditor = item->editor();
    if (!propEditor){
        return;
    }

    UsdAttributeWrapper* attrWrapper = propEditor->usdAttributeWrapper();
    if (!attrWrapper) {
        return;
    }

    PXR_NS::VtValue newValue;
    if (attrWrapper->get(newValue)) {
        // update the editor underlying value
        QVariant refreshed = propEditor->fromVtValue(newValue);
        propEditor->setCurrentValue(refreshed);

        // update the underlying value in the QStandardItem
        m_inUndoRedoRefresh = true;
        item->setData(refreshed, Qt::EditRole);
        m_inUndoRedoRefresh = false;
    }

    // repaint this index
    const QModelIndex nameIndex = this->index(index.row(), 0, index.parent());
    emit dataChanged(nameIndex, nameIndex, { Qt::DisplayRole, Qt::EditRole, Qt::FontRole, Qt::ForegroundRole });
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QStandardItemModel::flags(index);

    // make the first column non-editable
    if (index.column() == 0)
    {
        flags &= ~Qt::ItemIsEditable;
    }

    return flags;
}

void PropertyModel::loadUsdProperties()
{
    for (const auto& proprty : GlobalSelection::instance().prim().GetProperties())
    {
        if (proprty.Is<PXR_NS::UsdAttribute>())
        {
            PXR_NS::UsdAttribute attr = proprty.As<PXR_NS::UsdAttribute>();
            // TODO: until we have a proper grouping order, group attributes under the Prim Type
            addPropertyToGroup(
                QString::fromStdString(GlobalSelection::instance().prim().GetTypeName().GetString()), attr);
        }
    }
}

void PropertyModel::loadVariantSets()
{
    PXR_NS::UsdVariantSets variantSets = GlobalSelection::instance().prim().GetVariantSets();
    for (const auto& variantSetName : variantSets.GetNames())
    {
        auto variantSet = variantSets.GetVariantSet(variantSetName);
        addVariantSetsToGroup("Variants", variantSet);
    }
}

int32_t PropertyModel::propertyCount() const { return countAllProperties(invisibleRootItem()); }

void PropertyModel::reset()
{
    clear();
    setHorizontalHeaderLabels({ "Name", "Value" });
}

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

void PropertyModel::addProperty(const PXR_NS::UsdAttribute& usdAttr)
{
    AbstractPropertyEditor* abstractPropEditor = TINKERUSD_NS::createNewAttributeEditor(usdAttr);
    PropertyItem* nameItem = new PropertyItem(abstractPropEditor->name(), QVariant(), abstractPropEditor);
    PropertyItem* valueItem
        = new PropertyItem(QString(), abstractPropEditor->currentValue(), abstractPropEditor);
    appendRow({ nameItem, valueItem });
}

void PropertyModel::addPropertyToGroup(const QString& groupName, const PXR_NS::UsdAttribute& usdAttr)
{
    QStandardItem* groupItem = findOrCreateGroupItem(groupName);

    AbstractPropertyEditor* abstractPropEditor = TINKERUSD_NS::createNewAttributeEditor(usdAttr);
    PropertyItem* nameItem = new PropertyItem(abstractPropEditor->name(), QVariant(), abstractPropEditor);
    PropertyItem* valueItem
        = new PropertyItem(QString(), abstractPropEditor->currentValue(), abstractPropEditor);
    groupItem->appendRow({ nameItem, valueItem });
}

void PropertyModel::addVariantSetsToGroup(const QString& groupName, const PXR_NS::UsdVariantSet& variantSet)
{
    QStandardItem* groupItem = findOrCreateGroupItem(groupName);

    VariantSetEditor* variantSetEditor = TINKERUSD_NS::createVariantSetEditor(variantSet);
    if (variantSetEditor)
    {
        PropertyItem* nameItem = new PropertyItem(variantSetEditor->name(), QVariant(), variantSetEditor);
        PropertyItem* valueItem
            = new PropertyItem(QString(), variantSetEditor->currentValue(), variantSetEditor);
        groupItem->appendRow({ nameItem, valueItem });
    }
}

int32_t PropertyModel::countAllProperties(const QStandardItem* parentItem) const
{
    int32_t count = 0;
    for (int32_t row = 0; row < parentItem->rowCount(); ++row)
    {
        const QStandardItem* item = parentItem->child(row);
        if (item->rowCount() > 0)
        {
            count += countAllProperties(item);
        }
        else
        {
            count++;
        }
    }
    return count;
}

} // namespace TINKERUSD_NS
