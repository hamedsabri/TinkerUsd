#include "UsdDocument.h"

#include "ui/undoManager.h"
#include "undo/usdUndoManager.h"

#include <QMessageBox>
#include <QDebug>

namespace TINKERUSD_NS
{

UsdDocument::UsdDocument(QObject* parent)
    : QObject(parent)
{
    qDebug() << "[UsdDocument] Created.";
}

PXR_NS::UsdStageRefPtr UsdDocument::createNewStageInMemory()
{
    qDebug() << "[UsdDocument] Creating new in-memory stage...";

    m_stage = PXR_NS::UsdStage::CreateInMemory();

    if (m_stage)
    {
        qDebug() << "[UsdDocument] New stage created successfully.";
        emit stageOpened("untitled");

        // clear undo stack
        UndoManager::instance().undoStack()->clear();

        auto targetLayer = m_stage->GetEditTarget().GetLayer();
        UsdUndoManager::instance().trackLayerStates(targetLayer);

        qDebug() << "[UsdDocument] Undo stack cleared and tracking layer:"
                 << QString::fromStdString(targetLayer->GetIdentifier());
    }
    else
    {
        qCritical() << "[UsdDocument] Failed to create in-memory stage.";
        return nullptr;
    }

    return m_stage;
}

PXR_NS::UsdStageRefPtr UsdDocument::openStage(const QString& path)
{
    qDebug() << "[UsdDocument] Opening stage from file:" << path;

    m_stage = PXR_NS::UsdStage::Open(path.toStdString(), PXR_NS::UsdStage::LoadAll);

    if (m_stage)
    {
        qDebug() << "[UsdDocument] Stage opened successfully.";
        emit stageOpened(path);

        UndoManager::instance().undoStack()->clear();

        auto targetLayer = m_stage->GetEditTarget().GetLayer();
        UsdUndoManager::instance().trackLayerStates(targetLayer);

        qDebug() << "[UsdDocument] Undo stack cleared and tracking layer:"
                 << QString::fromStdString(targetLayer->GetIdentifier());
    }
    else
    {
        qCritical() << "[UsdDocument] Failed to open stage:" << path;
        return nullptr;
    }

    return m_stage;
}

PXR_NS::UsdStageRefPtr UsdDocument::getCurrentStage() const
{
    return m_stage;
}

PXR_NS::SdfLayerRefPtr UsdDocument::getRootLayer() const
{
    return m_stage->GetRootLayer();
}

void UsdDocument::setEditTargetLayer(PXR_NS::SdfLayerHandle layer)
{
    qDebug() << "[UsdDocument] Setting new edit target layer:"
             << QString::fromStdString(layer->GetIdentifier());

    m_stage->SetEditTarget(PXR_NS::UsdEditTarget(layer));
}

} // namespace TINKERUSD_NS
