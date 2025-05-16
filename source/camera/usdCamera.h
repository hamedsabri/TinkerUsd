#pragma once

#include <pxr/base/gf/camera.h>
#include <pxr/usd/usdGeom/bboxCache.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS
{

/*
Simple camera that is mostly taken from freeCamera.py in Pixar's UsdView.
There is a lot of features that is missing here.
*/
class UsdCamera final
{
public:
    enum class DragMode
    {
        NONE,
        DOLLY,
        PAN,
        ZOOM
    };

    UsdCamera(PXR_NS::UsdStageRefPtr stage);
    ~UsdCamera() = default;

    const GfCamera& getCamera() const;

    // returns view matrix
    GfMatrix4d getViewMatrix() const;

    // returns projection matrix
    GfMatrix4d getProjectionMatrix() const;

    GfBBox3d getStageBBox() const;

    void setBoundingBox(const GfBBox3d& bBox);

    // update the camera's transform matrix
    void updateTransform();

    // frame to stage bbox
    void frameBoundingBox();

    // Scales the distance of the UsdCamera from it's center
    void adjustDistance(double scaleFactor);

    double aspectRatio() const;
    void   setAspectRatio(double aspectRatio);

    DragMode getDragMode() const;
    void     setDragMode(DragMode dragMode);

    void dolly(double x, double y);
    void pan(double x, double y);
    void zoom(double zoomFactor);

    double computePixelsToWorldFactor(int height);

private:
    GfCamera m_camera;

    double m_fov;
    double m_distance;
    double m_nearClip;
    double m_farClip;
    double m_rotTheta; // pitch
    double m_rotPhi;   // roll
    double m_rotPsi;   // yaw
    double m_aspectRatio;

    GfBBox3d  m_bbox;
    TfToken   m_upAxis;
    GfVec3d   m_center;
    GfRange3d m_range;

    DragMode m_dragMode;
};

} // namespace TINKERUSD_NS