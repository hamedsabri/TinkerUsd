#include "viewportOpenGLWidget.h"
#include "core/globalSelection.h"
#include "core/usdDocument.h"

#include <QMouseEvent>
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

void ViewportOpenGLWidget::onSelectionChanged() { update(); }

void ViewportOpenGLWidget::onUsdObjectChanged(const UsdNotice::ObjectsChanged& notice)
{
    const auto& resyncedPaths = notice.GetResyncedPaths();
    const auto& changedPaths = notice.GetChangedInfoOnlyPaths();

    if (!resyncedPaths.empty() || !changedPaths.empty()) {
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
    if (!m_stage) {
        return;
    }

    m_usdCamera = std::make_unique<UsdCamera>(m_stage);
    m_renderEngineGL = std::make_unique<UsdRenderEngineGL>();
    m_renderEngineGL->initialize(m_stage, m_usdCamera.get());
}

void ViewportOpenGLWidget::onStageOpened(const QString& filePath)
{
    m_stage = m_usdDocument->getCurrentStage();

    initialize();
    update();
}

void ViewportOpenGLWidget::resizeGL(int w, int h)
{
    m_width = w * devicePixelRatio();
    m_height = h * devicePixelRatio();

    m_drawTarget->resize(m_width, m_height);

    glViewport(0, 0, w, h);
}

QString ViewportOpenGLWidget::rendererDisplayName() const
{
    return QString::fromStdString(m_renderEngineGL->rendererDisplayName());
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
    glDepthMask(GL_TRUE);

    switch (m_shadingMode)
    {
    case ShadingMode::POINTS:
        m_renderEngineGL->params().drawMode = UsdImagingGLDrawMode::DRAW_POINTS;
        break;
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
    m_renderEngineGL->params().cullStyle
        = UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
    m_renderEngineGL->params().clearColor = GfVec4f(0.2f, 0.2f, 0.2f, 1.0f);
    m_renderEngineGL->params().forceRefresh = false;
    m_renderEngineGL->params().enableLighting = true; // false to turn off camera light
    m_renderEngineGL->params().enableSampleAlphaToCoverage = false;
    m_renderEngineGL->params().enableSceneMaterials = false;
    m_renderEngineGL->params().enableSceneLights = true;
    m_renderEngineGL->params().flipFrontFacing = true;
    m_renderEngineGL->params().gammaCorrectColors = false;
    m_renderEngineGL->params().highlight = true;
    m_renderEngineGL->params().showGuides = true;
    m_renderEngineGL->params().showProxy = true;
    m_renderEngineGL->params().showRender = true;
    m_renderEngineGL->params().complexity = 1.0;

    m_renderEngineGL->updateSelection(GlobalSelection::instance().path());
    m_renderEngineGL->render(m_stage, m_usdCamera.get(), m_width, m_height);

    m_drawTarget->unbind();
    m_drawTarget->draw();
}

void ViewportOpenGLWidget::wheelEvent(QWheelEvent* event)
{
    double angleDelta = static_cast<double>(event->angleDelta().y()) / 1000.0;
    m_usdCamera->adjustDistance(1.0 - std::max(-0.5, std::min(0.5, angleDelta)));

    update();
}

void ViewportOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePosition = event->pos() * devicePixelRatioF();

    if (event->modifiers() & (Qt::AltModifier | Qt::MetaModifier)) {
        if (event->button() == Qt::LeftButton) {
            m_usdCamera->setDragMode(UsdCamera::DragMode::DOLLY);
        }
        else if (event->button() == Qt::RightButton) {
            m_usdCamera->setDragMode(UsdCamera::DragMode::ZOOM);
        }
        else if (event->button() == Qt::MiddleButton) {
            m_usdCamera->setDragMode(UsdCamera::DragMode::PAN);
        }
    }
}

void ViewportOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint currentMousePosition = event->pos() * devicePixelRatioF();

    QPoint delta = currentMousePosition - m_lastMousePosition;
    if (delta.x() == 0 && delta.y() == 0) {
        return;
    }

    if (m_usdCamera->getDragMode() == UsdCamera::DragMode::DOLLY) {
        m_usdCamera->dolly(0.25 * delta.x(), 0.25 * delta.y());
    }
    else if (m_usdCamera->getDragMode() == UsdCamera::DragMode::PAN) {
        auto pixelsToWorld = m_usdCamera->computePixelsToWorldFactor(m_height);
        m_usdCamera->pan(-delta.x() * pixelsToWorld, delta.y() * pixelsToWorld);
    }
    else if (m_usdCamera->getDragMode() == UsdCamera::DragMode::ZOOM) {
        auto zoomDelta = -.002 * (delta.x() + delta.y());
        m_usdCamera->zoom(zoomDelta);
    }

    m_lastMousePosition = event->pos() * devicePixelRatioF();

    update();
}

void ViewportOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_usdCamera->setDragMode(UsdCamera::DragMode::NONE);
}

} // namespace TINKERUSD_NS
