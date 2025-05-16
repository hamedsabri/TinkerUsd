#include "compositionInspectorWidget.h"

#include "core/globalSelection.h"
#include "core/usdDocument.h"

#include <QMainWindow>
#include <QVBoxLayout>

#include <pxr/usd/sdf/path.h>

namespace TINKERUSD_NS
{

CompositionInspectorWidget::CompositionInspectorWidget(UsdDocument* document, QMainWindow* parent)
    : QWidget(parent)
    , m_usdDocument(document)
{
    setObjectName("CompositionInspectorWidget");

    setupLayout();

    connect(
        m_usdDocument, &UsdDocument::stageOpened, this, &CompositionInspectorWidget::onStageOpened);

    connect(
        &GlobalSelection::instance(),
        &GlobalSelection::selectionChanged,
        this,
        &CompositionInspectorWidget::onSelectionChanged);
}

CompositionInspectorWidget::~CompositionInspectorWidget() { }

void CompositionInspectorWidget::setupLayout()
{
    // layer stack
    m_layerStackWidget = new LayerStack(this);

    // composition
    m_primCompositionWidget = new PrimComposition(this);

    // tab
    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->addTab(m_layerStackWidget, "Layer Stack");
    tabWidget->addTab(m_primCompositionWidget, "Composition");

    QVBoxLayout* mainLayoutV = new QVBoxLayout();

    mainLayoutV->setContentsMargins(0, 0, 0, 0);
    mainLayoutV->setSpacing(0);
    mainLayoutV->addWidget(tabWidget);
    setLayout(mainLayoutV);
}

void CompositionInspectorWidget::onStageOpened(const QString& filePath)
{
    m_layerStackWidget->clearAll();
    m_primCompositionWidget->clearAll();
}

void CompositionInspectorWidget::onSelectionChanged()
{
    m_layerStackWidget->clearAll();
    m_primCompositionWidget->clearAll();

    PXR_NS::UsdPrim selectedPrim = GlobalSelection::instance().prim();
    if (!selectedPrim.IsValid())
    {
        return;
    }

    m_layerStackWidget->setTableData(selectedPrim);
    m_primCompositionWidget->setTreeData(selectedPrim);
}

} // namespace TINKERUSD_NS