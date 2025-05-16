#include "primComposition.h"
#include "contentBrowser.h"

#include <QList>
#include <QAction>
#include <QClipboard>
#include <QPainter>
#include <QApplication>
#include <QHeaderView>
#include <QMenu>

#include <pxr/base/tf/token.h>
#include <pxr/usd/pcp/layerStack.h>
#include <pxr/usd/pcp/node.h>
#include <pxr/usd/sdf/layerTree.h>

Q_DECLARE_METATYPE(PXR_NS::SdfLayerHandle)

const QString kTextMessage("Select a prim in the scene to view composition arcs.");

namespace
{

QList<QString> rowValuesStr(
    const PXR_NS::PcpNodeRef&         node,
    const PXR_NS::SdfLayerTreeHandle& layerTree,
    bool                              sublayer = false)
{
    const auto& layer = layerTree->GetLayer();

    QString arcType { "" };
    switch (node.GetArcType())
    {
    case PXR_NS::PcpArcTypeRoot: arcType = QString("root"); break;
    case PXR_NS::PcpArcTypeReference: arcType = QString("reference"); break;
    case PXR_NS::PcpArcTypePayload: arcType = QString("payload"); break;
    case PXR_NS::PcpArcTypeInherit: arcType = QString("inherit"); break;
    case PXR_NS::PcpArcTypeSpecialize: arcType = QString("specialize"); break;
    case PXR_NS::PcpArcTypeVariant: arcType = QString("variant"); break;
    default: break;
    }

    if (sublayer)
    {
        arcType = QString("sublayer");
    }

    auto pathIntro = QString::fromStdString(node.GetPathAtIntroduction().GetString());
    auto hasSpec = static_cast<bool>((layer->GetObjectAtPath(node.GetPath()))) ? QString("yes")
                                                                               : QString("no");

    return { QString::fromStdString(layer->GetDisplayName()), arcType, pathIntro, hasSpec };
}

// create treeview items for all sublayers in the layer tree.
QTreeWidgetItem* walkSublayers(
    QTreeWidget*                      treeWidget,
    QTreeWidgetItem*                  parent,
    const PXR_NS::PcpNodeRef&         node,
    const PXR_NS::SdfLayerTreeHandle& layerTree,
    bool                              sublayer = false)
{
    QTreeWidgetItem* item;
    if (treeWidget)
    {
        item = new QTreeWidgetItem(
            treeWidget, QStringList { rowValuesStr(node, layerTree, sublayer) });
    }
    else
    {
        item = new QTreeWidgetItem(parent, QStringList { rowValuesStr(node, layerTree, sublayer) });
    }

    item->setExpanded(true);
    item->setToolTip(0, layerTree->GetLayer()->GetIdentifier().c_str());

    // setBackground
    auto spec = layerTree->GetLayer()->GetObjectAtPath(node.GetPath());
    if (!spec)
    {
        for (auto i = 0; i < item->columnCount(); ++i)
        {
            item->setBackground(i, QBrush(QColor(37, 37, 38)));
        }
    }

    // set data for context menu
    for (auto i = 0; i < item->columnCount(); ++i)
    {
        QVariant variant;
        variant.setValue(layerTree->GetLayer());
        item->setData(i, Qt::UserRole, variant);
    }

    for (auto subtree : layerTree->GetChildTrees())
    {
        walkSublayers(nullptr, item, node, subtree, true);
    }

    return item;
}

void walkNodes(QTreeWidget* treeWidget, QTreeWidgetItem* parent, const PXR_NS::PcpNodeRef& node)
{
    auto nodeItem = walkSublayers(treeWidget, parent, node, node.GetLayerStack()->GetLayerTree());

    for (const PXR_NS::PcpNodeRef& childNode : node.GetChildrenRange())
    {
        walkNodes(nullptr, nodeItem, childNode);
    }
}

} // namespace

namespace TINKERUSD_NS
{

PrimComposition::PrimComposition(QWidget* parent)
    : QTreeWidget(parent)
{
    setupLayout();
}

void PrimComposition::setTreeData(const PXR_NS::UsdPrim& prim)
{
    // populate the treeview with items from the prim index.
    if (prim.GetPrimIndex().IsValid())
    {
        walkNodes(this, nullptr, prim.GetPrimIndex().GetRootNode());
    }
}

void PrimComposition::clearAll() { clear(); }

void PrimComposition::setupLayout()
{
    setObjectName("PrimComposition");

    setHeaderLabels({ "Layer", "Arc Type", "Arc Path", "Has Spec" });

    // context menu
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this, &QTreeWidget::customContextMenuRequested, this, &PrimComposition::showContextMenu);

    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(2, QHeaderView::Interactive);
    header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    setStyleSheet("QTreeWidget::item {  height:20px }");
}

void PrimComposition::showContextMenu(const QPoint& pos)
{
    auto                   currentIndex = selectionModel()->currentIndex();
    auto                   variant = currentIndex.data(Qt::UserRole);
    PXR_NS::SdfLayerHandle layer = variant.value<PXR_NS::SdfLayerHandle>();
    if (layer == nullptr)
    {
        return;
    }

    QMenu contextMenu(this);

    // copy Layer Path
    QAction* copyLayerPathAction = new QAction(tr("&Copy Layer Path"), this);
    contextMenu.addAction(copyLayerPathAction);

    // browse content
    QAction* inspectAction = new QAction(tr("&Browse Content"), this);
    contextMenu.addAction(inspectAction);

    auto currentAction = contextMenu.exec(mapToGlobal(pos));

    // real path
    QClipboard* cb = QApplication::clipboard();
    if (currentAction == copyLayerPathAction)
    {
        cb->setText(QString::fromStdString(layer->GetRealPath()));
    }

    // browse content
    if (currentAction == inspectAction)
    {
        std::string layerContent;
        layer->ExportToString(&layerContent);
        ContentBrowser* contentBroswer
            = new ContentBrowser(QString::fromStdString(layerContent), this);
        contentBroswer->show();
    }
}

void PrimComposition::paintEvent(QPaintEvent* event)
{
    QTreeView::paintEvent(event);

    // draw a text message if model doesn't have any data
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