#include "outlinerView.h"

#include "core/globalSelection.h"
#include "outlinerModel.h"

#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLineEdit>

namespace TINKERUSD_NS
{

UsdOutlinerView::UsdOutlinerView(QWidget* parent)
    : QTreeView(parent)
    , m_model(new UsdOutlinerModel(this))
    , m_proxyModel(new UsdOutlinerFilterProxyModel(this))
    , m_searchLineEdit(nullptr)
{
    m_proxyModel->setSourceModel(m_model);
    setModel(m_proxyModel);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setItemsExpandable(true);
    setUniformRowHeights(true);
    setSortingEnabled(false);
    setMouseTracking(true);
    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setExpandsOnDoubleClick(false);

    auto p = palette();
    p.setColor(QPalette::Base, QColor("#373737"));
    setPalette(p);

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("Search...");
    m_searchLineEdit->setClearButtonEnabled(true);

    connect(
        selectionModel(), &QItemSelectionModel::selectionChanged, this, &UsdOutlinerView::onSelectionChanged);
    connect(m_searchLineEdit, &QLineEdit::textChanged, this, &UsdOutlinerView::onSearchTextChanged);


    connect(
        &GlobalSelection::instance(), &GlobalSelection::selectionChanged,
        this, [this](const PXR_NS::UsdPrim& prim) {
            if (prim.IsValid()) {
                focusPrim(prim);
            } else {
                selectionModel()->clearSelection();
            }
        });
}

void UsdOutlinerView::setStage(const PXR_NS::UsdStageRefPtr& stage)
{
    m_model->setStage(stage);

    if (m_searchLineEdit)
    {
        m_searchLineEdit->clear();
    }
}

void UsdOutlinerView::onSelectionChanged(const QItemSelection& selected)
{
    if (selected.indexes().isEmpty())
    {
        return;
    }

    QModelIndex index = selected.indexes().first().siblingAtColumn(0);
    PXR_NS::UsdPrim prim = m_proxyModel->primFromIndex(index);
    if (prim.IsValid())
    {
        GlobalSelection::instance().setPrim(prim);
    }
}

void UsdOutlinerView::onSearchTextChanged(const QString& text)
{
    m_proxyModel->setFilterPattern(text);

    if (!text.isEmpty())
    {
        expandAll();
    }
}

void UsdOutlinerView::focusPrim(const UsdPrim& prim)
{
    QModelIndex sourceIndex = m_model->indexFromPrim(prim);
    if (!sourceIndex.isValid())
    {
        return;
    }

    QModelIndex proxyIndex = m_proxyModel->mapFromSource(sourceIndex);
    if (!proxyIndex.isValid())
    {
        return;
    }

    expand(proxyIndex.parent());
    scrollTo(proxyIndex, QAbstractItemView::PositionAtCenter);
    setCurrentIndex(proxyIndex);
}

} // namespace TINKERUSD_NS