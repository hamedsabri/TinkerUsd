#include "propertyTreeView.h"

#include "common/utils.h"
#include "propertyContextMenu.h"
#include "propertyDelegate.h"
#include "propertyModel.h"
#include "propertyProxy.h"
#include "valueEditors/abstractPropertyEditor.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QVector3D>

const QString kTextMessage("Select a prim in the scene to view and edit its properties");

namespace TINKERUSD_NS
{

PropertyTreeView::PropertyTreeView(QWidget* parent)
    : QTreeView(parent)
    , m_model(new PropertyModel(this))
    , m_delegate(new PropertyDelegate(this))
    , m_proxyModel(new PropertyProxy(this))
{
    setUniformRowHeights(false);
    setIndentation(14);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::NoSelection);

    // header
    setHeaderHidden(false);
    // header()->setStretchLastSection(true);
    // header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // set model and delegate
    m_proxyModel->setSourceModel(m_model);
    setModel(m_proxyModel);
    setItemDelegate(m_delegate);

    // Disable editing for the first column (which is for group names)
    // header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    // header()->setStretchLastSection(true);
}

PropertyModel* PropertyTreeView::getModel() const { return m_model; }

PropertyProxy* PropertyTreeView::getProxyModel() const { return m_proxyModel; }

void PropertyTreeView::expandGroup(const QString& groupName)
{
    QList<QStandardItem*> items = m_model->findItems(groupName, Qt::MatchExactly);
    if (!items.isEmpty())
    {
        QModelIndex sourceIndex = m_model->indexFromItem(items.first());
        QModelIndex proxyIndex = TINKERUSD_NS::Utils::mapFromSourceIndex(m_proxyModel, sourceIndex);
        setExpanded(proxyIndex, true);
    }
}

void PropertyTreeView::expandAllGroups() { expandAll(); }

void PropertyTreeView::collapseGroup(const QString& groupName)
{
    QList<QStandardItem*> items = m_model->findItems(groupName, Qt::MatchExactly);
    if (!items.isEmpty())
    {
        QModelIndex sourceIndex = m_model->indexFromItem(items.first());
        QModelIndex proxyIndex = TINKERUSD_NS::Utils::mapFromSourceIndex(m_proxyModel, sourceIndex);
        setExpanded(proxyIndex, false);
    }
}

void PropertyTreeView::collapseAllGroups() { collapseAll(); }

void PropertyTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
    {
        return;
    }

    PropertyContextMenu menu(this, index, this);
    menu.exec(event->globalPos());
}

void PropertyTreeView::reapplyPersistentEditors()
{
    std::function<void(const QModelIndex&)> applyEditors
        = [this, &applyEditors](const QModelIndex& parentIndex) {
              for (int row = 0; row < m_model->rowCount(parentIndex); ++row)
              {
                  QModelIndex sourceNameIndex = m_model->index(row, 0, parentIndex);
                  QModelIndex sourceValueIndex = m_model->index(row, 1, parentIndex);

                  if (!sourceNameIndex.isValid() || !sourceValueIndex.isValid())
                      continue;

                  QModelIndex proxyValueIndex = m_proxyModel->mapFromSource(sourceValueIndex);
                  closePersistentEditor(proxyValueIndex); // TODO: do we need this?
                  openPersistentEditor(proxyValueIndex);

                  // recurse into children
                  applyEditors(sourceNameIndex);
              }
          };

    // apply to the top-level items
    applyEditors(QModelIndex());
}

void PropertyTreeView::paintEvent(QPaintEvent* event)
{
    QTreeView::paintEvent(event);

    if (model() && model()->rowCount() == 0)
    {
        QPainter painter(viewport());
        QRect    rect = viewport()->rect();
        QFont    bold;
        bold.setBold(true);
        painter.setFont(bold);
        painter.drawText(rect, Qt::AlignCenter, kTextMessage);
    }
}

} // namespace TINKERUSD_NS
