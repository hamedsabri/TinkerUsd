#pragma once

#include <QObject>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/usd/stage.h>

namespace TINKERUSD_NS
{

class UsdDocument : public QObject
{
    Q_OBJECT
public:
    UsdDocument(QObject* parent = nullptr);
    virtual ~UsdDocument() = default;

    PXR_NS::UsdStageRefPtr createNewStageInMemory();

    PXR_NS::UsdStageRefPtr openStage(const QString& path);

    void setEditTargetLayer(PXR_NS::SdfLayerHandle layer);

    PXR_NS::UsdStageRefPtr getCurrentStage() const;

    PXR_NS::SdfLayerRefPtr getCurrentLayer() const;

signals:
    void stageOpened(const QString& filePath);

private:
    PXR_NS::UsdStageRefPtr m_stage;
};

} // namespace TINKERUSD_NS