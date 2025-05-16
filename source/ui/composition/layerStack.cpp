#include "layerStack.h"

#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTableWidgetItem>
#include <map>
#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/value.h>
#include <pxr/usd/sdf/path.h>

Q_DECLARE_METATYPE(PXR_NS::SdfSpecHandle)

const QString kTextMessage("Select a prim in the scene to view stack of layers "
                           "that contribute opinions.");

namespace
{

QString formatMetadataValueView(const std::map<PXR_NS::TfToken, PXR_NS::VtValue>& metadataDict)
{
    QString itemStr;
    QString separator;

    for (const auto& [key, value] : metadataDict)
    {
        const QString keyStr = QString::fromStdString(key.GetString());
        const QString valueStr = QString::fromStdString(PXR_NS::TfStringify(value));

        if (!itemStr.isEmpty())
        {
            separator = ", ";
        }

        itemStr += separator + keyStr + ": " + valueStr;
    }

    return "{" + itemStr + "}";
}

QString metaData(const PXR_NS::SdfSpecHandle& spec)
{
    std::map<PXR_NS::TfToken, PXR_NS::VtValue> metadataDict;

    for (const auto& key : spec->GetMetaDataInfoKeys())
    {
        if (spec->HasInfo(key))
        {
            metadataDict[key] = spec->GetInfo(key);
        }
    }

    return formatMetadataValueView(metadataDict);
}

} // namespace

namespace TINKERUSD_NS
{

LayerStack::LayerStack(QWidget* parent)
    : QTableWidget(parent)
{
    setupLayout();
}

void LayerStack::setTableData(const PXR_NS::UsdPrim& prim)
{
    PXR_NS::SdfPrimSpecHandleVector specs;

    auto path = prim.GetPath();
    if (path.IsPropertyPath())
    {
        // HS TODO
        return;
    }
    else
    {
        specs = prim.GetPrimStack();
        setHorizontalHeaderItem(2, new QTableWidgetItem(QString(("Metadata"))));
    }

    setRowCount(specs.size());

    for (auto i = 0; i < specs.size(); ++i)
    {
        PXR_NS::SdfSpecHandle spec = specs[i];

        // layer
        auto layerItem = new QTableWidgetItem(QString(spec->GetLayer()->GetDisplayName().c_str()));
        setItem(i, 0, layerItem);

        // path
        auto pathItem = new QTableWidgetItem(QString(spec->GetPath().GetText()));
        setItem(i, 1, pathItem);

        // meta data
        if (path.IsPropertyPath())
        {
            // HS TODO:
            continue;
        }
        else
        {
            auto valueItem = new QTableWidgetItem(metaData(spec));
            setItem(i, 2, valueItem);
        }

        // set data for context menu
        for (auto j = 0; j < columnCount(); ++j)
        {
            QVariant variant;
            variant.setValue(spec);

            auto theItem = this->item(i, j);
            theItem->setData(Qt::UserRole, variant);
        }
    }
}

void LayerStack::clearAll()
{
    clearContents();
    setRowCount(0);
}

void LayerStack::setupLayout()
{
    setObjectName("LayerStack");

    setRowCount(0);
    setColumnCount(3);
    setHorizontalHeaderLabels({ "Layer", "Path", "MetaData" });
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    horizontalHeader()->setStretchLastSection(true);

    // context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableWidget::customContextMenuRequested, this, &LayerStack::showContextMenu);
}

void LayerStack::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu;
    contextMenu.setObjectName("LayerStackTableContextMenu");
    contextMenu.setSeparatorsCollapsible(false);

    // actions
    QAction* copyLayerPathAction = new QAction("Copy Layer Path", this);
    QAction* copyLayerIdentifierAction = new QAction("Copy Layer Identifier", this);
    QAction* copyMetaDataAction = new QAction("Copy MetaData", this);

    contextMenu.addAction(copyLayerPathAction);
    contextMenu.addAction(copyLayerIdentifierAction);
    contextMenu.addAction(copyMetaDataAction);

    // layer
    auto currentIndex = selectionModel()->currentIndex();
    auto dataModeItem = this->item(currentIndex.row(), currentIndex.column());
    if (dataModeItem == nullptr)
    {
        return;
    }

    auto                  variant = dataModeItem->data(Qt::UserRole);
    PXR_NS::SdfSpecHandle spec = variant.value<PXR_NS::SdfSpecHandle>();
    auto                  currentAction = contextMenu.exec(mapToGlobal(pos));

    QClipboard* cb = QApplication::clipboard();
    if (currentAction == copyLayerPathAction)
    {
        cb->setText(QString::fromStdString(spec->GetLayer()->GetRealPath()));
    }
    else if (currentAction == copyLayerIdentifierAction)
    {
        cb->setText(QString::fromStdString(spec->GetLayer()->GetIdentifier()));
    }
    else if (currentAction == copyMetaDataAction)
    {
        cb->setText(metaData(spec));
    }
}

void LayerStack::paintEvent(QPaintEvent* event)
{
    QTableView::paintEvent(event);

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