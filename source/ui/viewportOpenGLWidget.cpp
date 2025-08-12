#include "viewportOpenGLWidget.h"

#include "core/globalSelection.h"
#include "core/usdDocument.h"

#include <QMouseEvent>
#include <QSurfaceFormat>
#include <QWheelEvent>
#include <pxr/usd/usdGeom/metrics.h>

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
    if (m_ObjectsChangedKey.IsValid())
    {
        TfNotice::Revoke(m_ObjectsChangedKey);
    }
}

void ViewportOpenGLWidget::onSelectionChanged()
{
    m_renderEngineGL->addSelectionHighlighting();

    m_renderEngineGL->addBboxRenderParams(globalSelectionBbox(m_stage));

    update();
}

void ViewportOpenGLWidget::onUsdObjectChanged(const UsdNotice::ObjectsChanged& notice)
{
    const auto& resyncedPaths = notice.GetResyncedPaths();
    const auto& changedPaths = notice.GetChangedInfoOnlyPaths();

    if (!resyncedPaths.empty() || !changedPaths.empty())
    {

        m_renderEngineGL->addBboxRenderParams(globalSelectionBbox(m_stage));

        update();
    }
}

void ViewportOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    initialize();

    m_drawTarget = std::make_unique<UsdDrawTargetFBO>();

    emit rendererAvailable();
}

void ViewportOpenGLWidget::initialize()
{
    if (!m_stage)
    {
        return;
    }

    m_usdCamera = std::make_unique<UsdCamera>(m_stage);
    m_renderEngineGL = std::make_unique<UsdRenderEngineGL>();
    m_renderEngineGL->initialize(m_stage);

    m_grid = std::make_unique<Grid>();
    m_grid->initialize();

    m_hud.init(this);
}

void ViewportOpenGLWidget::onStageOpened(const QString& filePath)
{
    m_stage = m_usdDocument->getCurrentStage();

    initialize();

    update();
}

void ViewportOpenGLWidget::frameSelected()
{
    m_usdCamera->frameSelected(globalSelectionBbox(m_stage));

    update();
}

void ViewportOpenGLWidget::reset()
{
    m_usdCamera->reset();

    update();
}

void ViewportOpenGLWidget::resizeGL(int w, int h)
{
    m_width = w * devicePixelRatio();
    m_height = h * devicePixelRatio();

    m_drawTarget->resize(m_width, m_height);

    glViewport(0, 0, w, h);
}

QString ViewportOpenGLWidget::upAxisDisplayName() const
{
    TfToken stageUpAxis = PXR_NS::UsdGeomGetStageUpAxis(m_stage);
    return QString::fromStdString(stageUpAxis.GetString());
}

void ViewportOpenGLWidget::setShadingMode(ViewportOpenGLWidget::ShadingMode mode)
{
    m_shadingMode = mode;

    update();
}

ViewportOpenGLWidget::ShadingMode ViewportOpenGLWidget::shadingMode() const { return m_shadingMode; }

std::vector<std::string> ViewportOpenGLWidget::getRendererAovs() const
{
    return m_renderEngineGL->getRendererAovs();
}

void ViewportOpenGLWidget::setRendererAov(const std::string& name)
{
    m_renderEngineGL->setRendererAov(name);

    update();
}

void ViewportOpenGLWidget::paintGL()
{
    m_drawTarget->bind();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    m_renderEngineGL->params().cullStyle = UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;

    switch (m_shadingMode)
    {
    case ShadingMode::POINTS: m_renderEngineGL->params().drawMode = UsdImagingGLDrawMode::DRAW_POINTS; break;
    case ShadingMode::WIREFRAME:
        m_renderEngineGL->params().drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME;
        break;
    case ShadingMode::WIREFRAME_ON_SURFACE:
        m_renderEngineGL->params().drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME_ON_SURFACE;
        break;
    case ShadingMode::SHADED_FLAT:
        m_renderEngineGL->params().drawMode = UsdImagingGLDrawMode::DRAW_SHADED_FLAT;
        break;
    case ShadingMode::SHADEDSMOOTH:
    default: m_renderEngineGL->params().drawMode = UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH; break;
    }
    m_renderEngineGL->params().cullStyle = UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
    m_renderEngineGL->params().clearColor = GfVec4f(0.2f, 0.2f, 0.2f, 1.0f);
    m_renderEngineGL->params().forceRefresh = false;
    m_renderEngineGL->params().enableLighting = true; // false to turn off camera light
    m_renderEngineGL->params().enableSampleAlphaToCoverage = false;
    m_renderEngineGL->params().enableSceneMaterials = true;
    m_renderEngineGL->params().enableSceneLights = true;
    m_renderEngineGL->params().flipFrontFacing = true;
    m_renderEngineGL->params().gammaCorrectColors = false;
    m_renderEngineGL->params().highlight = true;
    m_renderEngineGL->params().showGuides = true;
    m_renderEngineGL->params().showProxy = true;
    m_renderEngineGL->params().showRender = true;
    m_renderEngineGL->params().complexity = 1.0;

    m_renderEngineGL->render(m_stage, m_usdCamera.get(), m_width, m_height);

    TfToken stageUpAxis = PXR_NS::UsdGeomGetStageUpAxis(m_stage);
    m_grid->draw(m_usdCamera.get(), stageUpAxis);

    m_drawTarget->unbind();
    m_drawTarget->draw();

    if (m_showRendererStats) {
        hudDrawRendereStats();
    }
}

void ViewportOpenGLWidget::setShowRendererStats(bool val)
{
    m_showRendererStats = val;

    update();
}

void ViewportOpenGLWidget::wheelEvent(QWheelEvent* event)
{
    double angleDelta = static_cast<double>(event->angleDelta().y()) / 1000.0;
    m_usdCamera->adjustDistance(1.0 - std::max(-0.5, std::min(0.5, angleDelta)));

    update();
}

void ViewportOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePosition = event->pos() * devicePixelRatio();

    if (event->modifiers() & (Qt::AltModifier | Qt::MetaModifier))
    {
        if (event->button() == Qt::LeftButton)
        {
            m_usdCamera->setDragMode(UsdCamera::DragMode::DOLLY);
        }
        else if (event->button() == Qt::RightButton)
        {
            m_usdCamera->setDragMode(UsdCamera::DragMode::ZOOM);
        }
        else if (event->button() == Qt::MiddleButton)
        {
            m_usdCamera->setDragMode(UsdCamera::DragMode::PAN);
        }
    }
    else
    {

        // NOTE: Explicitly set Depth Mask to True
        // OtherWise TestIntersection fails to pick
        glDepthMask(GL_TRUE);

        // normalize position and pick size by the viewport size
        auto pos = pxr::GfVec2d(m_lastMousePosition.x() / m_width, m_lastMousePosition.y() / m_height);

        pos[0] = (pos[0] * 2.0 - 1.0);
        pos[1] = -1.0 * (pos[1] * 2.0 - 1.0);

        auto size = pxr::GfVec2d(1.0f / m_width, 1.0f / m_height);
        auto cameraFrustum = m_usdCamera->getCamera().GetFrustum();
        auto pickFrustum = cameraFrustum.ComputeNarrowedFrustum(pos, size);

        pxr::GfVec3d outHitNormal;
        pxr::GfVec3d outHitPoint;
        pxr::SdfPath outHitInstancerPath;
        pxr::SdfPath outHitPrimPath;
        auto hit = m_renderEngineGL->getUsdImagingGLEngine()->TestIntersection(
            pickFrustum.ComputeViewMatrix(),
            pickFrustum.ComputeProjectionMatrix(),
            m_stage->GetPseudoRoot(),
            m_renderEngineGL->params(),
            &outHitPoint,
            &outHitNormal,
            &outHitPrimPath,
            &outHitInstancerPath);
        if (hit)
        {
            auto hitPrim = m_stage->GetPrimAtPath(outHitPrimPath);
            GlobalSelection::instance().setPrim(hitPrim);
        }
        else
        {
            GlobalSelection::instance().clearSelection();
        }
    }
}

void ViewportOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint currentMousePosition = event->pos() * devicePixelRatio();

    QPoint delta = currentMousePosition - m_lastMousePosition;
    if (delta.x() == 0 && delta.y() == 0)
    {
        return;
    }

    if (m_usdCamera->getDragMode() == UsdCamera::DragMode::DOLLY)
    {
        m_usdCamera->dolly(0.25 * delta.x(), 0.25 * delta.y());
    }
    else if (m_usdCamera->getDragMode() == UsdCamera::DragMode::PAN)
    {
        auto pixelsToWorld = m_usdCamera->computePixelsToWorldFactor(m_height);
        m_usdCamera->pan(-delta.x() * pixelsToWorld, delta.y() * pixelsToWorld);
    }
    else if (m_usdCamera->getDragMode() == UsdCamera::DragMode::ZOOM)
    {
        auto zoomDelta = -.002 * (delta.x() + delta.y());
        m_usdCamera->zoom(zoomDelta);
    }

    m_lastMousePosition = event->pos() * devicePixelRatio();

    update();
}

void ViewportOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_usdCamera->setDragMode(UsdCamera::DragMode::NONE);
}

double ViewportOpenGLWidget::nearClip() const
{
    return m_usdCamera ? m_usdCamera->nearClip() : 0.01;
}

double ViewportOpenGLWidget::farClip() const
{
    return m_usdCamera ? m_usdCamera->farClip() : 1000000.0;
}

void ViewportOpenGLWidget::setClipPlanes(double nearClip, double farClip)
{
    if (!m_usdCamera) {
        return;
    }

    m_usdCamera->setClippingRange(nearClip, farClip);
    m_usdCamera->updateTransform();

    update();
}

void ViewportOpenGLWidget::hudDrawRendereStats()
{
    QStringList lines;

    lines << QStringLiteral("Renderer: %1").arg(QString::fromStdString(m_renderEngineGL->rendererDisplayName()));
  
    auto hgiApiName = m_renderEngineGL->getUsdImagingGLEngine()->GetHgi()->GetAPIName();
    lines << QStringLiteral("Hgi: %1").arg(QString::fromStdString(hgiApiName.GetString()));

    lines << "==================== ";
    lines << "Render Statistics: ";
    lines << "==================== ";

    auto renderStatsDict = m_renderEngineGL->getUsdImagingGLEngine()->GetRenderStats();
    for (const auto& kv : renderStatsDict) {
        const std::string& key = kv.first;
        const VtValue& value   = kv.second;

        lines << QStringLiteral("%1 = %2")
                     .arg(QString::fromStdString(key))
                     .arg(QString::fromStdString(TfStringify(value)));
    }

    QString hudText = lines.join("\n");

    m_hud.updateText(hudText, float(devicePixelRatioF()), 14);
    m_hud.draw(width(), height(), float(devicePixelRatioF()));
}


} // namespace TINKERUSD_NS
