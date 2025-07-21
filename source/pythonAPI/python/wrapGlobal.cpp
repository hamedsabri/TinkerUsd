#include "../global.h"

#include <pxr/external/boost/python.hpp>
#include <pxr/external/boost/python/wrapper.hpp>

using namespace PXR_BOOST_PYTHON_NAMESPACE;

void wrapGlobal() { def("stage", TINKERUSD_NS::stage); }
