#include "usdAttributeWrapper.h"

#include "common/utils.h"

namespace TINKERUSD_NS
{

UsdAttributeWrapper::UsdAttributeWrapper(const PXR_NS::UsdAttribute& usdAttr)
    : m_usdAttr(usdAttr)
{
}

UsdAttributeWrapper::Ptr UsdAttributeWrapper::create(const PXR_NS::UsdAttribute& usdAttr)
{
    return std::unique_ptr<UsdAttributeWrapper>(new UsdAttributeWrapper(usdAttr));
}

bool UsdAttributeWrapper::get(PXR_NS::VtValue& value, PXR_NS::UsdTimeCode time) const
{
    return m_usdAttr.Get(&value, time);
}

bool UsdAttributeWrapper::set(const PXR_NS::VtValue& value, PXR_NS::UsdTimeCode time)
{
    return m_usdAttr.Set(value, time);
}

bool UsdAttributeWrapper::isAuthored() const { return isValid() && m_usdAttr.IsAuthored(); }

bool UsdAttributeWrapper::isValid() const { return m_usdAttr.IsValid(); }

bool UsdAttributeWrapper::hasValue() const { return isValid() ? m_usdAttr.HasValue() : false; }

QString UsdAttributeWrapper::nativeType() const
{
    return QString::fromStdString(usdAttributeType().GetType().GetTypeName());
}

QString UsdAttributeWrapper::displayName() const
{
    auto attributeOriginalName = name().GetString();
    auto attributeSanitizedName = TINKERUSD_NS::Utils::removePrefix(attributeOriginalName, "_");
    auto attributeName = attributeSanitizedName ? *attributeSanitizedName : attributeOriginalName;

    return QString::fromStdString(attributeName.data());
}

QString UsdAttributeWrapper::documentation() const
{
    if (isValid())
    {
        return QString::fromStdString(m_usdAttr.GetDocumentation());
    }
    else
    {
        return QString();
    }
}

PXR_NS::TfToken UsdAttributeWrapper::name() const
{
    if (isValid())
    {
        return m_usdAttr.GetName();
    }
    else
    {
        return TfToken();
    }
}

QString UsdAttributeWrapper::path() const
{
    return QString::fromStdString(m_usdAttr.GetPath().GetString());
}

PXR_NS::UsdPrim UsdAttributeWrapper::usdPrim() const { return m_usdAttr.GetPrim(); }

PXR_NS::UsdAttribute UsdAttributeWrapper::usdAttribute() const { return m_usdAttr; }

PXR_NS::SdfValueTypeName UsdAttributeWrapper::usdAttributeType() const
{
    return m_usdAttr.GetTypeName();
}

} // namespace TINKERUSD_NS
