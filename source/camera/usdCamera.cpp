#include "usdCamera.h"

#include <pxr/base/gf/frustum.h>
#include <pxr/imaging/cameraUtil/framing.h>
#include <pxr/usd/usdGeom/bboxCache.h>
#include <pxr/usd/usdGeom/metrics.h>

#define PI 3.14159265358979323846f

namespace TINKERUSD_NS
{

UsdCamera::UsdCamera(PXR_NS::UsdStageRefPtr stage)
    : m_stage(stage)
    , m_upAxis(PXR_NS::UsdGeomGetStageUpAxis(m_stage))
{
    reset();
}

void UsdCamera::initialize()
{
    m_fov = 60.0;
    m_distance = 100.0;
    m_nearClip = 1.0;
    m_farClip = 1000000.0;
    m_rotTheta = 0;
    m_rotPhi = 0;
    m_rotPsi = 0;
    m_aspectRatio = 1.0;

    UsdGeomBBoxCache bboxCache(UsdTimeCode::Default(), UsdGeomImageable::GetOrderedPurposeTokens());
    setBoundingBox(bboxCache.ComputeWorldBound(m_stage->GetPseudoRoot()));

    m_camera.SetPerspectiveFromAspectRatioAndFieldOfView(m_aspectRatio, m_fov, GfCamera::FOVVertical);
    m_camera.SetFocusDistance(m_distance);
    m_camera.SetClippingRange(GfRange1f(m_nearClip, m_farClip));
    CameraUtilConformWindowPolicy policy = CameraUtilConformWindowPolicy::CameraUtilFit;
    CameraUtilConformWindow(&m_camera, policy, m_aspectRatio);

    frameBoundingBox();
}

const GfCamera& UsdCamera::getCamera() const { return m_camera; }

GfMatrix4d UsdCamera::getViewMatrix() const { return m_camera.GetFrustum().ComputeViewMatrix(); }

GfMatrix4d UsdCamera::getProjectionMatrix() const { return m_camera.GetFrustum().ComputeProjectionMatrix(); }

void UsdCamera::setBoundingBox(const GfBBox3d& bBox)
{
    if (m_bbox != bBox)
    {
        m_bbox = bBox;
        m_center = bBox.ComputeCentroid();
        m_range = bBox.ComputeAlignedRange();
    }
}

void UsdCamera::updateTransform()
{
    GfMatrix4d matrix = GfMatrix4d().SetTranslate(GfVec3d().ZAxis() * m_distance);
    matrix *= GfMatrix4d(1.0).SetRotate(GfRotation(GfVec3d().ZAxis(), -m_rotPsi));
    matrix *= GfMatrix4d(1.0).SetRotate(GfRotation(GfVec3d().XAxis(), -m_rotPhi));
    matrix *= GfMatrix4d(1.0).SetRotate(GfRotation(GfVec3d().YAxis(), -m_rotTheta));

    GfMatrix4d matrixUp;
    if (m_upAxis == TfToken("X"))
    {
        matrixUp.SetRotate(GfRotation(GfVec3d::YAxis(), -90.0));
    }
    else if (m_upAxis == TfToken("Z"))
    {
        matrixUp.SetRotate(GfRotation(GfVec3d::XAxis(), -90.0));
    }
    else
    {
        matrixUp = GfMatrix4d(1.0);
    }

    matrix *= matrixUp.GetInverse();

    matrix *= GfMatrix4d().SetTranslate(m_center);
    m_camera.SetTransform(matrix);

    if (m_camera.GetProjection() == GfCamera::Perspective)
    {
        // reset projection
        m_camera.SetPerspectiveFromAspectRatioAndFieldOfView(m_aspectRatio, m_fov, GfCamera::FOVVertical);
        m_camera.SetFocusDistance(m_distance);
        m_camera.SetClippingRange(GfRange1f(m_nearClip, m_farClip));
    }
}

void UsdCamera::frameBoundingBox()
{
    if (m_camera.GetProjection() == GfCamera::Perspective)
    {
        // calculate distance
        auto size = m_range.GetSize();
        auto maxsize = std::max(size[0], std::max(size[1], size[2]));
        auto fov = m_fov * 0.5;
        if (m_fov == 0.0)
        {
            m_fov = 0.5;
        }
        auto lengthToFit = maxsize * 0.5;
        m_distance = lengthToFit / std::atan(fov * (PI / 180.0));
        if (m_distance < m_nearClip + maxsize * 0.5)
        {
            m_distance = m_nearClip + lengthToFit;
        }
    }
}

void UsdCamera::adjustDistance(double scaleFactor)
{
    // When dist gets very small, you can get stuck and not be able to
    // zoom back out, if you just keep multiplying.  Switch to addition
    // in that case, choosing an incr that works for the scale of the
    // framed geometry.
    if (scaleFactor > 1 && m_distance < 2)
    {
        auto rangeSize = m_bbox.ComputeAlignedRange().GetSize();
        auto maxRangeSize = std::max(rangeSize[0], std::max(rangeSize[1], rangeSize[2]));
        m_distance += std::min(maxRangeSize / 25.0, scaleFactor);
    }
    else
    {
        m_distance *= scaleFactor;
    }
}

double UsdCamera::aspectRatio() const { return m_aspectRatio; }

void UsdCamera::setAspectRatio(double aspectRatio) { m_aspectRatio = aspectRatio; }

UsdCamera::DragMode UsdCamera::getDragMode() const { return m_dragMode; }

void UsdCamera::setDragMode(UsdCamera::DragMode dragMode) { m_dragMode = dragMode; }

void UsdCamera::dolly(double x, double y)
{
    m_rotTheta += x;
    m_rotPhi += y;
}

void UsdCamera::pan(double x, double y)
{
    auto frustum = m_camera.GetFrustum();
    auto up = frustum.ComputeUpVector().GetNormalized();
    auto forward = frustum.ComputeViewDirection().GetNormalized();
    auto right = GfCross(forward, up);
    m_center += x * right + y * up;
}

void UsdCamera::zoom(double zoomFactor)
{
    if (m_camera.GetProjection() == GfCamera::Perspective)
    {
        adjustDistance(1 + zoomFactor);
    }
}

double UsdCamera::computePixelsToWorldFactor(int height)
{
    if (m_camera.GetProjection() == GfCamera::Perspective)
    {
        auto frustum = m_camera.GetFrustum();
        auto frustumHeight = frustum.GetWindow().GetSize()[1];
        return frustumHeight * m_distance / height;
    }

    return 0;
}

void UsdCamera::frameSelected(const GfBBox3d& bBox)
{
    setBoundingBox(bBox);
    frameBoundingBox();
}

void UsdCamera::reset()
{
    initialize();
    frameBoundingBox();
}

} // namespace TINKERUSD_NS