#pragma once

#include <pxr/imaging/glf/simpleLight.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>
#include <pxr/usdImaging/usdImagingGL/renderParams.h>

#include <vector>
#include <memory>

namespace TINKERUSD_NS
{
class UsdCamera;
class UsdRenderEngineGL final
{
    using Ptr = std::unique_ptr<PXR_NS::UsdImagingGLEngine>;

public:
    UsdRenderEngineGL() = default;
    ~UsdRenderEngineGL() = default;

    void initialize(const PXR_NS::UsdStageRefPtr& stage, UsdCamera* camera);
    void render(const PXR_NS::UsdStageRefPtr& stage, UsdCamera* camera, double w, double h);
    void updateSelection(const PXR_NS::SdfPath& path);

    PXR_NS::UsdImagingGLRenderParams& params();
    pxr::UsdImagingGLEngine* getUsdImagingGLEngine() const; 

    std::vector<std::string> getRendererAovs() const;
    void                     setRendererAov(const std::string& name);

    std::string rendererDisplayName() const;

private:
    Ptr                              m_usdGLEngine;
    PXR_NS::UsdImagingGLRenderParams m_params;
    PXR_NS::UsdStageRefPtr           m_stage;

    PXR_NS::GlfSimpleLight       m_cameraLight;
    PXR_NS::GlfSimpleLightVector m_lights;
    PXR_NS::GlfSimpleMaterial    m_material;
    PXR_NS::GfVec4f              m_ambient;

    std::string m_aov { "color" };
};

} // namespace TINKERUSD_NS
