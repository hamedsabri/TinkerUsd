#pragma once

#include "../api.h"

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>

namespace TINKERUSD_NS
{

TINKERUSD_PUBLIC
PXR_NS::UsdPrim selectedPrim();

#define DISALLOW_COPY_MOVE_ASSIGNMENT(ClassName)     \
    ClassName(const ClassName&) = delete;            \
    ClassName& operator=(const ClassName&) = delete; \
    ClassName(const ClassName&&) = delete;           \
    ClassName&& operator=(const ClassName&&) = delete;

} // namespace TINKERUSD_NS