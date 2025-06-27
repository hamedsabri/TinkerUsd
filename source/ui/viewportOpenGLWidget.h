#pragma once

#include "camera/usdCamera.h"
#include "core/utils.h"
#include "render/usdDrawTargetFBO.h"
#include "render/usdRenderEngineGL.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#include <QString>
#include <QVector>

#include <pxr/base/tf/notice.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/stage.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS
{
class UsdDocument;
class ViewportOpenGLWidget
    : public QOpenGLWidget
    , public QOpenGLFunctions_4_5_Core
    , public PXR_NS::TfWeakBase
{
    Q_OBJECT
public:
    enum class ViewportLayoutMode
    {
        SINGLE,
        TWO_UP_HORIZONTAL,
        TWO_UP_VERTICAL,
        FOUR_UP
    };
    Q_ENUM(ViewportLayoutMode)

    enum class ShadingMode
    {
        SHADEDSMOOTH,
        POINTS,
        WIREFRAME,
        WIREFRAME_ON_SURFACE,
        SHADED_FLAT
    };

    Q_ENUM(ShadingMode)

    ViewportOpenGLWidget(UsdDocument* document, QWidget* parent = nullptr);
    virtual ~ViewportOpenGLWidget();

    DISALLOW_COPY_MOVE_ASSIGNMENT(ViewportOpenGLWidget);

    QString rendererDisplayName() const;

    void setShadingMode(ShadingMode mode);
    ShadingMode shadingMode() const;

    void setRendererAov(const std::string& name);
    std::vector<std::string> getRendererAovs() const;

    void setViewportLayoutMode(ViewportLayoutMode mode);
    ViewportLayoutMode viewportLayoutMode() const;

    void setLightingEnabled(bool enabled);
    bool isLightingEnabled() const;

    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void initialize();
    void onUsdObjectChanged(const UsdNotice::ObjectsChanged& notice);
    void onSelectionChanged();

Q_SIGNALS:
    void rendererAvailable();

private slots:
    void onStageOpened(const QString& filePath);

private:
    UsdDocument*                       m_usdDocument;
    std::unique_ptr<UsdCamera>         m_usdCamera;
    std::unique_ptr<UsdDrawTargetFBO>  m_drawTarget;
    std::unique_ptr<UsdRenderEngineGL> m_renderEngineGL;
    PXR_NS::UsdStageRefPtr             m_stage;
    QPoint                             m_lastMousePosition;
    TfNotice::Key                      m_ObjectsChangedKey;
    double                             m_height;
    double                             m_width;
    ShadingMode                        m_shadingMode;
    ViewportLayoutMode                 m_viewportLayoutMode;
    std::vector<QRect>                 m_viewportRects;
    int                                m_activeViewportIndex; // For now, primarily for input routing

    // Store multiple instances for multi-viewport layouts
    std::vector<std::unique_ptr<UsdCamera>>         m_cameras;
    std::vector<std::unique_ptr<UsdDrawTargetFBO>>  m_drawTargets;
    std::vector<std::unique_ptr<UsdRenderEngineGL>> m_renderEngines;

    bool                               m_lightingEnabled;
    bool                               m_gridEnabled;

    UsdCamera* getActiveCamera();
    UsdRenderEngineGL* getActiveRenderEngine();
    int getViewportIndexForPoint(const QPoint& point);
    void updateViewportRects();
    void initializeViewportResources(int count);
};

} // namespace TINKERUSD_NS