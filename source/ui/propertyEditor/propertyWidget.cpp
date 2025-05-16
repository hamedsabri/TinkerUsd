#include "propertyWidget.h"

#include "common/utils.h"
#include "core/globalSelection.h"
#include "core/usdDocument.h"
#include "modelView/propertyModel.h"
#include "modelView/propertyProxy.h"
#include "modelView/propertyTreeView.h"
#include "searchBar.h"
#include "valueEditors/factory.h"

#include <QtCore/QTimer>
#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

PropertyWidget::PropertyWidget(UsdDocument* document, QWidget* parent)
    : QWidget(parent)
    , m_usdDocument(document)
{
    onCreateUI();

    connect(m_usdDocument, &UsdDocument::stageOpened, this, &PropertyWidget::onStageOpened);

    connect(
        &GlobalSelection::instance(),
        &GlobalSelection::selectionChanged,
        this,
        &PropertyWidget::onSelectionChanged);
}

void PropertyWidget::onCreateUI()
{
    m_treeView = new PropertyTreeView(this);
    m_searchBar = new SearchBar(m_treeView, this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);

    mainLayout->addWidget(m_searchBar);
    mainLayout->addWidget(m_treeView);
    setLayout(mainLayout);
}

void PropertyWidget::onStageOpened(const QString& filePath) { m_treeView->getModel()->reset(); }

void PropertyWidget::onSelectionChanged()
{
    m_treeView->getModel()->reset();

    PXR_NS::UsdPrim selectedPrim = GlobalSelection::instance().prim();
    if (!selectedPrim.IsValid())
    {
        return;
    }

    m_treeView->getModel()->setUsdPrim(selectedPrim);
    m_treeView->getModel()->loadUsdAttributes();
    m_treeView->getModel()->loadVariantSets();
    m_treeView->expandAllGroups();

    m_treeView->reapplyPersistentEditors();
}

} // namespace TINKERUSD_NS