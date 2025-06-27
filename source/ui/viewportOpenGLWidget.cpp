#include "viewportOpenGLWidget.h"
#include "core/globalSelection.h"
#include "core/usdDocument.h"

#include <QMouseEvent>
#include <QPainter>
#include <QSurfaceFormat>
#include <QWheelEvent>

#define SAMPLE_AMOUNT 8

namespace TINKERUSD_NS
{

ViewportOpenGLWidget::ViewportOpenGLWidget(UsdDocument* document, QWidget* parent)
    : QOpenGLWidget(parent)
    , m_usdDocument(document)
    , m_stage(document->createNewStageInMemory())
    , m_height(1)
    , m_width(1)
    , m_shadingMode(ShadingMode::SHADEDSMOOTH)
    , m_viewportLayoutMode(ViewportLayoutMode::SINGLE)
    , m_activeViewportIndex(0)
    , m_lightingEnabled(true)
    , m_gridEnabled(false) // Default grid to off
{
    QSurfaceFormat format;
    format.setSamples(SAMPLE_AMOUNT);
    setFormat(format);

    TfWeakPtr<ViewportOpenGLWidget> me(this);
    m_ObjectsChangedKey = TfNotice::Register(me, &ViewportOpenGLWidget::onUsdObjectChanged);

    connect(m_usdDocument, &UsdDocument::stageOpened, this, &ViewportOpenGLWidget::onStageOpened);

    connect(
        &GlobalSelection::instance(),
        &GlobalSelection::selectionChanged,
        this,
        &ViewportOpenGLWidget::onSelectionChanged);
}

ViewportOpenGLWidget::~ViewportOpenGLWidget()
{
    if (m_ObjectsChangedKey.IsValid()) {
        TfNotice::Revoke(m_ObjectsChangedKey);
    }
}

void ViewportOpenGLWidget::onSelectionChanged()
{
    if (auto* renderEngine = getActiveRenderEngine()) {
        renderEngine->addSelectionHighlighting();
        renderEngine->addBboxRenderParams(globalSelectionBbox(m_stage));
    }
    update();
}

void ViewportOpenGLWidget::onUsdObjectChanged(const UsdNotice::ObjectsChanged& notice)
{
    const auto& resyncedPaths = notice.GetResyncedPaths();
    const auto& changedPaths = notice.GetChangedInfoOnlyPaths();

    if (!resyncedPaths.empty() || !changedPaths.empty()) {
        if (auto* renderEngine = getActiveRenderEngine()) {
            renderEngine->addBboxRenderParams(globalSelectionBbox(m_stage));
        }
        update();
    }
}

void ViewportOpenGLWidget::initializeViewportResources(int count)
{
    m_cameras.clear();
    m_renderEngines.clear();
    m_drawTargets.clear(); // Assuming one FBO per viewport for simplicity first

    for (int i = 0; i < count; ++i) {
        if (m_stage) {
            m_cameras.emplace_back(std::make_unique<UsdCamera>(m_stage));
            m_renderEngines.emplace_back(std::make_unique<UsdRenderEngineGL>());
            m_renderEngines.back()->initialize(m_stage);
        } else {
            // Handle case where stage is not yet available if necessary
            m_cameras.emplace_back(nullptr); // Or some default camera
            m_renderEngines.emplace_back(nullptr);
        }
        m_drawTargets.emplace_back(std::make_unique<UsdDrawTargetFBO>());
    }
    // Ensure active index is valid
    m_activeViewportIndex = std::min(m_activeViewportIndex, count - 1);
    if (m_activeViewportIndex < 0 && count > 0) m_activeViewportIndex = 0;

}


void ViewportOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    initializeViewportResources(1); // Start with a single viewport
    updateViewportRects();

    if (!m_drawTargets.empty() && m_drawTargets[0]) {
        // emit rendererAvailable for the first viewport or a general one
        // This might need adjustment based on how renderers are managed per viewport
    }
    emit rendererAvailable(); // May need to be more specific if renderers differ
}

void ViewportOpenGLWidget::initialize() // This method might be redundant or need refocusing
{
    // Original content:
    // if (!m_stage) {
    //     return;
    // }
    // m_usdCamera = std::make_unique<UsdCamera>(m_stage);
    // m_renderEngineGL = std::make_unique<UsdRenderEngineGL>();
    // m_renderEngineGL->initialize( m_stage );
    //
    // This logic is now largely in initializeViewportResources
    // We might re-initialize if the stage changes significantly.
    if (m_stage) {
        setViewportLayoutMode(m_viewportLayoutMode); // Re-init resources for current layout
    }
}


void ViewportOpenGLWidget::onStageOpened(const QString& filePath)
{
    m_stage = m_usdDocument->getCurrentStage();
    // Re-initialize resources for all viewports with the new stage
    initializeViewportResources(m_cameras.size());
    update();
}

void ViewportOpenGLWidget::updateViewportRects() {
    m_viewportRects.clear();
    int w = width(); // Use widget's current width and height
    int h = height();

    switch (m_viewportLayoutMode) {
        case ViewportLayoutMode::SINGLE:
            m_viewportRects.push_back(QRect(0, 0, w, h));
            break;
        case ViewportLayoutMode::TWO_UP_HORIZONTAL:
            m_viewportRects.push_back(QRect(0, 0, w / 2, h));
            m_viewportRects.push_back(QRect(w / 2, 0, w / 2, h));
            break;
        case ViewportLayoutMode::TWO_UP_VERTICAL:
            m_viewportRects.push_back(QRect(0, 0, w, h / 2));
            m_viewportRects.push_back(QRect(0, h / 2, w, h / 2));
            break;
        case ViewportLayoutMode::FOUR_UP:
            m_viewportRects.push_back(QRect(0, 0, w / 2, h / 2));
            m_viewportRects.push_back(QRect(w / 2, 0, w / 2, h / 2));
            m_viewportRects.push_back(QRect(0, h / 2, w / 2, h / 2));
            m_viewportRects.push_back(QRect(w / 2, h / 2, w / 2, h / 2));
            break;
    }
}

void ViewportOpenGLWidget::resizeGL(int w, int h)
{
    // m_width and m_height are pixel dimensions, w and h are window coordinates
    // The devicePixelRatio is already handled by QOpenGLWidget context if needed
    // For FBOs and glViewport, we usually use pixel dimensions.
    m_width = w * devicePixelRatio();
    m_height = h * devicePixelRatio();

    updateViewportRects();

    for (size_t i = 0; i < m_drawTargets.size(); ++i) {
        if (m_drawTargets[i] && i < m_viewportRects.size()) {
             // Resize FBO to the size of its corresponding viewport rect
            m_drawTargets[i]->resize(m_viewportRects[i].width() * devicePixelRatio(), m_viewportRects[i].height() * devicePixelRatio());
        }
    }
    // glViewport will be set per-viewport in paintGL
}

QString ViewportOpenGLWidget::rendererDisplayName() const
{
    if (auto* renderEngine = getActiveRenderEngine()) {
        return QString::fromStdString(renderEngine->rendererDisplayName());
    }
    return "N/A";
}

ViewportOpenGLWidget::ViewportLayoutMode ViewportOpenGLWidget::viewportLayoutMode() const
{
    return m_viewportLayoutMode;
}

void ViewportOpenGLWidget::setViewportLayoutMode(ViewportOpenGLWidget::ViewportLayoutMode mode)
{
    if (m_viewportLayoutMode == mode) return;

    m_viewportLayoutMode = mode;
    int numViewports = 1;
    switch (mode) {
        case ViewportLayoutMode::SINGLE:
            numViewports = 1;
            break;
        case ViewportLayoutMode::TWO_UP_HORIZONTAL:
        case ViewportLayoutMode::TWO_UP_VERTICAL:
            numViewports = 2;
            break;
        case ViewportLayoutMode::FOUR_UP:
            numViewports = 4;
            break;
    }

    // If the number of viewports changes, re-initialize resources.
    // A more sophisticated approach might try to preserve existing cameras/settings.
    if (m_cameras.size() != numViewports && m_stage) {
         // Basic strategy: if cameras exist, copy the first one's state to new ones.
        GfCamera* oldPrimaryCamState = nullptr;
        if (!m_cameras.empty() && m_cameras[0]) {
            oldPrimaryCamState = &m_cameras[0]->getCamera();
        }

        initializeViewportResources(numViewports);

        if (oldPrimaryCamState && numViewports > 1) {
            for (size_t i = 0; i < m_cameras.size(); ++i) {
                if (m_cameras[i]) {
                    // This is a shallow copy of GfCamera state. Deeper copy might be needed.
                    // m_cameras[i]->setCameraState(*oldPrimaryCamState); // Need a method for this
                    // For now, they will re-initialize to default framing their bounding box.
                    // A better approach would be to copy transform, FoV, clipping, etc.
                    if (i > 0 && !m_cameras.empty() && m_cameras[0]) { // copy from the first camera
                        m_cameras[i]->setBoundingBox(m_cameras[0]->getCamera().GetFrustum().ComputeAABB()); // A bit of a hack
                        m_cameras[i]->setTransform(m_cameras[0]->getCamera().GetTransform());
                        // Copy other relevant params from m_cameras[0] to m_cameras[i]
                    }
                }
            }
        }
    }

    updateViewportRects();
    // resizeGL will be called by Qt if widget size changes,
    // but if only layout changes, we might need to trigger FBO resize if their sizes depend on layout.
    // Call resizeGL explicitly to handle FBO resizing based on new view rectangles.
    resizeGL(width(), height());


    update(); // Trigger repaint
}


void ViewportOpenGLWidget::setShadingMode(ViewportOpenGLWidget::ShadingMode mode)
{
    m_shadingMode = mode;
    update();
}

ViewportOpenGLWidget::ShadingMode ViewportOpenGLWidget::shadingMode() const
{
    return m_shadingMode;
}

void ViewportOpenGLWidget::setLightingEnabled(bool enabled) {
    if (m_lightingEnabled == enabled) return;
    m_lightingEnabled = enabled;
    update();
}

bool ViewportOpenGLWidget::isLightingEnabled() const {
    return m_lightingEnabled;
}

void ViewportOpenGLWidget::setGridEnabled(bool enabled) {
    if (m_gridEnabled == enabled) return;
    m_gridEnabled = enabled;
    update();
}

bool ViewportOpenGLWidget::isGridEnabled() const {
    return m_gridEnabled;
}

std::vector<std::string> ViewportOpenGLWidget::getRendererAovs() const
{
    // Ensure we try to get AOVs from a valid render engine,
    // especially if called before full initialization or if multi-engine setups differ.
    if (!m_renderEngines.empty() && m_renderEngines[0]) {
        return m_renderEngines[0]->getRendererAovs();
    }
    // Fallback for the case where m_renderEngineGL might still exist from old code or single-view setup
    if (m_renderEngineGL) {
        return m_renderEngineGL->getRendererAovs();
    }
    return {}; // Return empty if no engine available
}

void ViewportOpenGLWidget::setRendererAov(const std::string& name)
{
    // Apply to all render engines, or just the active one?
    // For now, apply to all, assuming they are similar.
    for (auto& engine : m_renderEngines) {
        if (engine) {
            engine->setRendererAov(name);
        }
    }
    update();
}

UsdCamera* ViewportOpenGLWidget::getActiveCamera() {
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < m_cameras.size() && m_cameras[m_activeViewportIndex]) {
        return m_cameras[m_activeViewportIndex].get();
    }
    return nullptr; // Should not happen if logic is correct
}

UsdRenderEngineGL* ViewportOpenGLWidget::getActiveRenderEngine() {
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < m_renderEngines.size() && m_renderEngines[m_activeViewportIndex]) {
        return m_renderEngines[m_activeViewportIndex].get();
    }
    return nullptr; // Should not happen
}

int ViewportOpenGLWidget::getViewportIndexForPoint(const QPoint& point) {
    for (size_t i = 0; i < m_viewportRects.size(); ++i) {
        if (m_viewportRects[i].contains(point)) {
            return static_cast<int>(i);
        }
    }
    return 0; // Default to first viewport if not found (e.g. click on border)
}


void ViewportOpenGLWidget::paintGL()
{
    if (m_viewportRects.empty() || m_cameras.empty() || m_renderEngines.empty() || m_drawTargets.empty()) {
        // Not fully initialized yet
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // A different color to indicate problem
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    makeCurrent(); // Ensure GL context is current for this widget

    for (size_t i = 0; i < m_viewportRects.size(); ++i) {
        if (i >= m_cameras.size() || !m_cameras[i] ||
            i >= m_renderEngines.size() || !m_renderEngines[i] ||
            i >= m_drawTargets.size() || !m_drawTargets[i]) {
            continue; // Skip if resources for this viewport are missing
        }

        const QRect& viewRect = m_viewportRects[i];
        UsdCamera* currentCamera = m_cameras[i].get();
        UsdRenderEngineGL* currentRenderEngine = m_renderEngines[i].get();
        UsdDrawTargetFBO* currentDrawTarget = m_drawTargets[i].get();

        // Set viewport for current sub-view
        // glViewport arguments are (x, y, width, height)
        // QRect gives (left, top, width, height). OpenGL y is from bottom.
        glViewport(viewRect.x() * devicePixelRatio(),
                   (height() - viewRect.bottom()) * devicePixelRatio(), // Correct Y for OpenGL
                   viewRect.width() * devicePixelRatio(),
                   viewRect.height() * devicePixelRatio());

        glScissor(viewRect.x() * devicePixelRatio(),
                  (height() - viewRect.bottom()) * devicePixelRatio(),
                  viewRect.width() * devicePixelRatio(),
                  viewRect.height() * devicePixelRatio());
        glEnable(GL_SCISSOR_TEST);


        currentDrawTarget->bind();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        currentRenderEngine->params().cullStyle = UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;

        switch (m_shadingMode) {
        case ShadingMode::POINTS:
            currentRenderEngine->params().drawMode = UsdImagingGLDrawMode::DRAW_POINTS;
            break;
        case ShadingMode::WIREFRAME:
            currentRenderEngine->params().drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME;
            break;
        case ShadingMode::WIREFRAME_ON_SURFACE:
            currentRenderEngine->params().drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME_ON_SURFACE;
            break;
        case ShadingMode::SHADED_FLAT:
            currentRenderEngine->params().drawMode = UsdImagingGLDrawMode::DRAW_SHADED_FLAT;
            break;
        case ShadingMode::SHADEDSMOOTH:
        default:
            currentRenderEngine->params().drawMode = UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH;
            break;
        }
        currentRenderEngine->params().clearColor = GfVec4f(0.2f, 0.2f, 0.2f, 1.0f);
        currentRenderEngine->params().forceRefresh = false;
        currentRenderEngine->params().enableLighting = true; // false to turn off camera light
        currentRenderEngine->params().enableSampleAlphaToCoverage = false;
    currentRenderEngine->params().enableLighting = m_lightingEnabled;
    currentRenderEngine->params().enableSampleAlphaToCoverage = false;
    currentRenderEngine->params().enableSceneMaterials = false;
    currentRenderEngine->params().enableSceneLights = true; // This usually refers to USD lights, not the camera/headlamp
    currentRenderEngine->params().flipFrontFacing = true;
        currentRenderEngine->params().gammaCorrectColors = false;
    currentRenderEngine->params().highlight = true; // Enable selection highlighting for all viewports

    // Assuming HdxRendererTaskParams structure for grid:
    // These names are typical based on Hydra's HdxRendererTaskParams.
    // If these exact names don't exist, this will need adjustment.
    currentRenderEngine->params().showGrid = m_gridEnabled;
    if (m_gridEnabled) {
        // Sensible defaults, could be made configurable later
        currentRenderEngine->params().gridColor = GfVec4f(0.5f, 0.5f, 0.5f, 0.5f);
        // currentRenderEngine->params().gridDivisions = 10; // Example if such param exists
        // currentRenderEngine->params().gridSpacing = GfVec2f(1.0f, 1.0f); // Example
    }

    currentRenderEngine->params().showGuides = true; // Keep other guides if any
        currentRenderEngine->params().showProxy = true;
        currentRenderEngine->params().showRender = true;
        currentRenderEngine->params().complexity = 1.0;

        // Pass viewport-specific width and height for aspect ratio calculation, etc.
        currentRenderEngine->render(m_stage, currentCamera, viewRect.width() * devicePixelRatio(), viewRect.height() * devicePixelRatio());

        currentDrawTarget->unbind();

        // Before drawing target, set viewport to where it should be drawn on the main framebuffer
        glViewport(viewRect.x() * devicePixelRatio(),
                   (height() - viewRect.bottom()) * devicePixelRatio(),
                   viewRect.width() * devicePixelRatio(),
                   viewRect.height() * devicePixelRatio());
        // Scissor should still be active for this draw
        currentDrawTarget->draw(); // Draw the FBO content to this viewport rect

        glDisable(GL_SCISSOR_TEST);
    }
    // Reset viewport to full widget size after drawing all sub-viewports, or Qt might get confused
    glViewport(0, 0, m_width, m_height);

    // Draw active viewport border using QPainter
    if (m_activeViewportIndex >= 0 && m_activeViewportIndex < m_viewportRects.size()) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::yellow, 2)); // Yellow border, 2px thick
        painter.drawRect(m_viewportRects[m_activeViewportIndex]);
    }
}

void ViewportOpenGLWidget::wheelEvent(QWheelEvent* event)
{
    m_activeViewportIndex = getViewportIndexForPoint(event->position().toPoint());
    UsdCamera* cam = getActiveCamera();
    if (!cam) return;

    double angleDelta = static_cast<double>(event->angleDelta().y()) / 1000.0;
    cam->adjustDistance(1.0 - std::max(-0.5, std::min(0.5, angleDelta)));

    update();
}

void ViewportOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    QPoint localPos = event->pos();
    m_activeViewportIndex = getViewportIndexForPoint(localPos);
    UsdCamera* cam = getActiveCamera();
    UsdRenderEngineGL* engine = getActiveRenderEngine();

    if (!cam || !engine) return;

    m_lastMousePosition = localPos * devicePixelRatio(); // devicePixelRatio already part of localPos from QEvent? Check Qt docs. Usually QEvent::pos() is logical.

    if (event->modifiers() & (Qt::AltModifier | Qt::MetaModifier)) {
        if (event->button() == Qt::LeftButton) {
            cam->setDragMode(UsdCamera::DragMode::ORBIT);
        } else if (event->button() == Qt::RightButton) {
            cam->setDragMode(UsdCamera::DragMode::ZOOM);
        } else if (event->button() == Qt::MiddleButton) {
            cam->setDragMode(UsdCamera::DragMode::PAN);
        }
    } else {
        glDepthMask(GL_TRUE); // Ensure depth mask is true for picking

        // Normalize mouse position relative to the *active sub-viewport*
        const QRect& activeViewRect = m_viewportRects[m_activeViewportIndex];
        QPoint relativeMousePos = localPos - activeViewRect.topLeft();

        double normX = static_cast<double>(relativeMousePos.x()) / activeViewRect.width();
        double normY = static_cast<double>(relativeMousePos.y()) / activeViewRect.height();

        // Convert to normalized device coordinates (-1 to 1, Y up)
        double ndcX = (normX * 2.0) - 1.0;
        double ndcY = -1.0 * ((normY * 2.0) - 1.0); // Y is inverted

        // Pick size should also be relative to the sub-viewport
        double pickWidthNDC = 2.0 / activeViewRect.width();
        double pickHeightNDC = 2.0 / activeViewRect.height();
        
        auto cameraFrustum = cam->getCamera().GetFrustum();
        auto pickFrustum = cameraFrustum.ComputeNarrowedFrustum(GfVec2d(ndcX, ndcY), GfVec2d(pickWidthNDC, pickHeightNDC));

        pxr::GfVec3d outHitNormal;
        pxr::GfVec3d outHitPoint;
        pxr::SdfPath outHitInstancerPath;
        pxr::SdfPath outHitPrimPath;
        auto hit = engine->getUsdImagingGLEngine()->TestIntersection(
            pickFrustum.ComputeViewMatrix(),
            pickFrustum.ComputeProjectionMatrix(),
            m_stage->GetPseudoRoot(),
            engine->params(),
            &outHitPoint,
            &outHitNormal,
            &outHitPrimPath,
            &outHitInstancerPath);

        if (hit) {
            auto hitPrim = m_stage->GetPrimAtPath(outHitPrimPath);
            GlobalSelection::instance().setPrim(hitPrim);
        } else {
            GlobalSelection::instance().clearSelection();
        }
    }
    update(); // Update to reflect selection or if active viewport changes visual state
}

void ViewportOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    // No need to update m_activeViewportIndex here, drag operations continue on the viewport they started in.
    UsdCamera* cam = getActiveCamera();
    if (!cam || cam->getDragMode() == UsdCamera::DragMode::NONE) {
        // Could add logic here for hover effects if needed, determining viewport by event->pos()
        return;
    }

    QPoint currentMousePosition = event->pos() * devicePixelRatio(); // Similar to press, check devicePixelRatio necessity
    QPoint delta = currentMousePosition - m_lastMousePosition;
    if (delta.x() == 0 && delta.y() == 0) {
        return;
    }

    if (cam->getDragMode() == UsdCamera::DragMode::ORBIT) {
        cam->orbit(0.25 * delta.x(), 0.25 * delta.y());
    } else if (cam->getDragMode() == UsdCamera::DragMode::PAN) {
        // Pan sensitivity might need to be adjusted based on the active sub-viewport's height
        const QRect& activeViewRect = m_viewportRects[m_activeViewportIndex];
        auto pixelsToWorld = cam->computePixelsToWorldFactor(activeViewRect.height() * devicePixelRatio());
        cam->pan(-delta.x() * pixelsToWorld, delta.y() * pixelsToWorld);
    } else if (cam->getDragMode() == UsdCamera::DragMode::ZOOM) {
        auto zoomDelta = -.002 * (delta.x() + delta.y());
        cam->zoom(zoomDelta);
    }

    m_lastMousePosition = currentMousePosition;
    update();
}

void ViewportOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    UsdCamera* cam = getActiveCamera();
    if (cam) {
        cam->setDragMode(UsdCamera::DragMode::NONE);
    }
    // m_activeViewportIndex could be reset here if desired, or kept until next press
    update();
}

} // namespace TINKERUSD_NS
