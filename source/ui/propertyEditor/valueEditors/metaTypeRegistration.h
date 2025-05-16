#pragma once

#include <QMetaType>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/vt/array.h>

namespace TINKERUSD_NS
{

Q_DECLARE_METATYPE(PXR_NS::VtArray<bool>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<int>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<float>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<double>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec2f>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec2d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec3f>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::GfVec3d>)
Q_DECLARE_METATYPE(PXR_NS::VtArray<PXR_NS::TfToken>)

// Add more declarations here as needed

} // namespace TINKERUSD_NS