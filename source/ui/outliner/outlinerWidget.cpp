#include "outlinerWidget.h"

#include "core/globalSelection.h"
#include "core/usdDocument.h"
#include "outlinerView.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usdGeom/imageable.h>

using namespace PXR_NS;

namespace TINKERUSD_NS
{

OutlinerWidget::OutlinerWidget(UsdDocument* document, QWidget* parent)
    : QWidget(parent)
    , m_usdDocument(document)
{
    onCreateUI();

    connect(m_usdDocument, &UsdDocument::stageOpened, this, &OutlinerWidget::onStageOpened);
}

void OutlinerWidget::onCreateUI()
{
    m_treeView = new UsdOutlinerView(this);

    m_searchBox = m_treeView->searchLineEdit();

    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->setContentsMargins(2, 2, 2, 2);
    searchLayout->addWidget(m_searchBox, 1); // 1 = stretch factor

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(4);

    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(m_treeView);

    setLayout(mainLayout);
}

void OutlinerWidget::onStageOpened(const QString& filePath)
{
    m_treeView->setStage(m_usdDocument->getCurrentStage());
}

} // namespace TINKERUSD_NS
