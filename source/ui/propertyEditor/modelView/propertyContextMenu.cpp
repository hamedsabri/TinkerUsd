#include "propertyContextMenu.h"

#include "common/utils.h"
#include "propertyModel.h"
#include "propertyProxy.h"
#include "propertyTreeView.h"
#include "valueEditors/abstractPropertyEditor.h"
#include "valueEditors/arrayEditor.h"
#include "valueEditors/variantSetEditor.h"

#include <QtGui/QClipboard>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtWidgets/QApplication>
#include <limits> // For std::numeric_limits

namespace TINKERUSD_NS
{

PropertyContextMenu::PropertyContextMenu(
    PropertyTreeView*  treeView,
    const QModelIndex& index,
    QWidget*           parent)
    : QMenu(parent)
    , m_treeView(treeView)
{
    QModelIndex sourceIndex = Utils::mapToSourceIndex(m_treeView->getProxyModel(), index);

    QStandardItem* item = m_treeView->getModel()->itemFromIndex(sourceIndex);
    if (item && item->rowCount() > 0)
    {
        return;
    }

    QAction* resetToDefaultAction = new QAction("Reset to Default", this);
    QAction* copyPropertyValueAction = new QAction("Copy Property Value", this);
    QAction* copyPropertyPathAction = new QAction("Copy Property Path", this);
    QAction* copyPropertyTypeAction = new QAction("Copy Property Type", this);

    // copyPropertyPath / copyPropertyType actions are irrelevant for variants, hence disabling them
    // here.
    AbstractPropertyEditor* abstractPropEditor = Utils::getEditorFromIndex(sourceIndex);
    if (dynamic_cast<VariantSetEditor*>(abstractPropEditor) != nullptr)
    {
        copyPropertyValueAction->setEnabled(false);
        copyPropertyPathAction->setEnabled(false);
        copyPropertyTypeAction->setEnabled(false);
    }

    // resetToDefaultAction is irrelevant for array editor
    if (dynamic_cast<ArrayEditorBase*>(abstractPropEditor) != nullptr)
    {
        resetToDefaultAction->setEnabled(false);
    }

    addAction(resetToDefaultAction);
    addSeparator();
    addAction(copyPropertyValueAction);
    addAction(copyPropertyPathAction);
    addAction(copyPropertyTypeAction);

    connect(resetToDefaultAction, &QAction::triggered, [this, index]() { resetToDefault(index); });
    connect(copyPropertyValueAction, &QAction::triggered, [this, index]() { copyPropertyValue(index); });
    connect(copyPropertyPathAction, &QAction::triggered, [this, index]() { copyPropertyPath(index); });
    connect(copyPropertyTypeAction, &QAction::triggered, [this, index]() { copyPropertyType(index); });
}

void PropertyContextMenu::resetToDefault(const QModelIndex& index)
{
    QModelIndex sourceIndex = TINKERUSD_NS::Utils::mapToSourceIndex(m_treeView->getProxyModel(), index);
    QModelIndex sourceValueIndex = m_treeView->getModel()->index(sourceIndex.row(), 1, sourceIndex.parent());

    AbstractPropertyEditor* abstractPropEditor = Utils::getEditorFromIndex(sourceValueIndex);
    if (!abstractPropEditor)
    {
        return;
    }

    abstractPropEditor->setCurrentValue(abstractPropEditor->defaultValue());
    m_treeView->getModel()->setData(sourceValueIndex, abstractPropEditor->defaultValue(), Qt::EditRole);
}

void PropertyContextMenu::copyPropertyValue(const QModelIndex& index)
{
    QModelIndex sourceIndex = TINKERUSD_NS::Utils::mapToSourceIndex(m_treeView->getProxyModel(), index);
    AbstractPropertyEditor* abstractPropEditor = Utils::getEditorFromIndex(sourceIndex);

    if (!abstractPropEditor)
    {
        return;
    }

    QString valueStr;
    if (ArrayEditorBase* arrayEditorBase = dynamic_cast<ArrayEditorBase*>(abstractPropEditor))
    {
        valueStr = arrayEditorBase->getRawArrayValue();
    }
    else
    {
        const QVariant value = abstractPropEditor->currentValue();
        // Using max_digits10 to produce the same floating-point value,
        // even though the intermediate text representation is not exact
        constexpr auto precision = std::numeric_limits<float>::max_digits10;

        if (value.canConvert<QVector2D>())
        {
            QVector2D vector = value.value<QVector2D>();
            valueStr = QString::fromStdString(
                std::format("[{:.{}g}, {:.{}g}]", vector.x(), precision, vector.y(), precision));
        }
        else if (value.canConvert<QVector3D>())
        {
            QVector3D vector = value.value<QVector3D>();
            valueStr = QString::fromStdString(std::format(
                "[{:.{}g}, {:.{}g}, {:.{}g}]",
                vector.x(),
                precision,
                vector.y(),
                precision,
                vector.z(),
                precision));
        }
        else
        {
            valueStr = value.toString();
        }
    }

    QApplication::clipboard()->setText(valueStr);
}

void PropertyContextMenu::copyPropertyPath(const QModelIndex& index)
{
    QModelIndex sourceIndex = TINKERUSD_NS::Utils::mapToSourceIndex(m_treeView->getProxyModel(), index);

    AbstractPropertyEditor* abstractPropEditor = Utils::getEditorFromIndex(sourceIndex);
    if (!abstractPropEditor)
    {
        return;
    }

    QApplication::clipboard()->setText(abstractPropEditor->usdAttributeWrapper()->path());
}

void PropertyContextMenu::copyPropertyType(const QModelIndex& index)
{
    QModelIndex sourceIndex = TINKERUSD_NS::Utils::mapToSourceIndex(m_treeView->getProxyModel(), index);

    AbstractPropertyEditor* abstractPropEditor = Utils::getEditorFromIndex(sourceIndex);
    if (!abstractPropEditor)
    {
        return;
    }

    QApplication::clipboard()->setText(abstractPropEditor->usdAttributeWrapper()->nativeType());
}

} // namespace TINKERUSD_NS