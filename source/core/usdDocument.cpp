#include "UsdDocument.h"

#include <QMessageBox>

namespace TINKERUSD_NS
{

UsdDocument::UsdDocument(QObject* parent)
    : QObject(parent)
{
}

PXR_NS::UsdStageRefPtr UsdDocument::createNewStageInMemory()
{
    m_stage = PXR_NS::UsdStage::CreateInMemory();

    if (m_stage) {
        emit stageOpened("untitled");
    }
    else {
        // TODO: Log.Error
        return nullptr;
    }

    return m_stage;
}

PXR_NS::UsdStageRefPtr UsdDocument::openStage(const QString& path)
{
    m_stage = PXR_NS::UsdStage::Open(path.toStdString(), PXR_NS::UsdStage::LoadAll);

    if (m_stage) {
        emit stageOpened(path);
    }
    else {
        // TODO: Log.Error
        return nullptr;
    }

    return m_stage;
}

PXR_NS::UsdStageRefPtr UsdDocument::getCurrentStage() const 
{ 
    return m_stage; 
}

PXR_NS::SdfLayerRefPtr UsdDocument::getCurrentLayer() const 
{ 
    return m_stage->GetRootLayer(); 
}

void UsdDocument::setEditTargetLayer(PXR_NS::SdfLayerHandle layer)
{
    m_stage->SetEditTarget(PXR_NS::UsdEditTarget(layer));
}

} // namespace TINKERUSD_NS