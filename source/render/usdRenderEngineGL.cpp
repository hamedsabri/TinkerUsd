#include "usdRenderEngineGL.h"

#include "camera/usdCamera.h"

#include <pxr/usd/usd/prim.h>
#include <pxr/usdImaging/usdImaging/delegate.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS
{

void UsdRenderEngineGL::initialize(const PXR_NS::UsdStageRefPtr& stage, UsdCamera* camera)
{
    PXR_NS::SdfPathVector excludedPaths;
    m_usdGLEngine = std::make_unique<PXR_NS::UsdImagingGLEngine>(
        stage->GetPseudoRoot().GetPath(), excludedPaths);

    // bbox
    m_params.bboxes.clear();
    m_params.bboxes.emplace_back(camera->getStageBBox());
    m_params.bboxLineColor = GfVec4f(1.0f, 1.0f, 1.0f, 1.0f); // White
    m_params.bboxLineDashSize = 3;

    // camera light
    m_cameraLight.SetAmbient({ 0.1, 0.1, 0.1, 1.0 });
    m_cameraLight.SetDiffuse({ 1.0, 1.0, 1.0, 1.f });
    m_cameraLight.SetSpecular({ 0.1, 0.1, 0.1, 1.f });
    m_cameraLight.SetPosition({ 10, 10, 10, 1.0 });

    m_material.SetAmbient({ 0.0, 0.0, 0.0, 1.f });
    m_material.SetDiffuse({ 1.0, 1.0, 1.0, 1.f });
    m_material.SetSpecular({ 0.1, 0.1, 0.1, 1.f });

    m_ambient = GfVec4f(0.0, 0.0, 0.0, 0.0);

    m_lights.push_back(m_cameraLight);
}

void UsdRenderEngineGL::render(
    const PXR_NS::UsdStageRefPtr& stage,
    UsdCamera*                 camera,
    double                     w,
    double                     h)
{
    camera->updateTransform();

    m_usdGLEngine->SetCameraState(camera->getViewMatrix(), camera->getProjectionMatrix());

    m_usdGLEngine->SetRenderBufferSize(GfVec2i(w, h));
    m_usdGLEngine->SetFraming(
        CameraUtilFraming(GfRange2f(GfVec2i(), GfVec2i(w, h)), GfRect2i(GfVec2i(), GfVec2i(w, h))));
    m_usdGLEngine->SetRenderViewport(GfVec4d(0, 0, w, h));
    m_usdGLEngine->SetWindowPolicy(CameraUtilMatchVertically);
    m_usdGLEngine->SetOverrideWindowPolicy(
        std::make_optional(CameraUtilConformWindowPolicy::CameraUtilMatchHorizontally));

    // update camera light position
    GfVec3d cameraPos = camera->getCamera().GetFrustum().GetPosition();
    m_cameraLight.SetPosition(GfVec4f(cameraPos[0], cameraPos[1], cameraPos[2], 1.0));
    m_cameraLight.SetTransform(camera->getCamera().GetTransform());

    // update the light state
    m_lights[0] = m_cameraLight;
    m_usdGLEngine->SetLightingState(m_lights, m_material, m_ambient);

    m_usdGLEngine->SetRendererAov(TfToken(m_aov));
    m_usdGLEngine->Render(stage->GetPseudoRoot(), m_params);
}

void UsdRenderEngineGL::updateSelection(const PXR_NS::SdfPath& path)
{
    m_usdGLEngine->ClearSelected();
    m_usdGLEngine->SetSelectionColor(PXR_NS::GfVec4f(1.0f, 1.0f, 0.0f, 0.5f));
    m_usdGLEngine->AddSelected(path, PXR_NS::UsdImagingDelegate::ALL_INSTANCES);
}

PXR_NS::UsdImagingGLRenderParams& UsdRenderEngineGL::params() 
{ 
    return m_params; 
}

pxr::UsdImagingGLEngine* UsdRenderEngineGL::getUsdImagingGLEngine() const
{ 
    return m_usdGLEngine.get(); 
}

std::string UsdRenderEngineGL::rendererDisplayName() const
{
    return m_usdGLEngine->GetRendererDisplayName(m_usdGLEngine->GetCurrentRendererId());
}

std::vector<std::string> UsdRenderEngineGL::getRendererAovs() const
{
    std::vector<std::string> result;
    for (const auto& aov : m_usdGLEngine->GetRendererAovs()) {
        result.emplace_back(aov);
    }
    return result;
}

void UsdRenderEngineGL::setRendererAov(const std::string& name) 
{ 
    m_aov = name; 
}

} // namespace TINKERUSD_NS
