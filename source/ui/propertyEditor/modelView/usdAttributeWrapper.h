#pragma once

#include <QtCore/QString>
#include <memory>
#include <pxr/usd/usd/attribute.h>

namespace TINKERUSD_NS
{

/**
 * @class UsdAttributeWrapper
 * @brief A wrapper class that provides convenient access and manipulation of USD attributes.
 */
class UsdAttributeWrapper final
{
public:
    using Ptr = std::unique_ptr<UsdAttributeWrapper>;

    //! constructor
    UsdAttributeWrapper(const PXR_NS::UsdAttribute& usdAttr);

    //! destructor
    ~UsdAttributeWrapper() = default;

    //! creates a new UsdAttributeWrapper instance.
    static Ptr create(const PXR_NS::UsdAttribute& usdAttr);

    //! retrieves the value of the USD attribute at a specific time code.
    bool get(PXR_NS::VtValue& value, PXR_NS::UsdTimeCode time = PXR_NS::UsdTimeCode::Default()) const;

    //! sets the value of the USD attribute at a specific time code.
    bool set(const PXR_NS::VtValue& value, PXR_NS::UsdTimeCode time = PXR_NS::UsdTimeCode::Default());

    //! checks whether the USD attribute has authored data.
    bool isAuthored() const;

    //! checks whether the USD attribute is valid.
    bool isValid() const;

    //! checks whether the USD attribute has a defined value.
    bool hasValue() const;

    //! return the native type name
    QString nativeType() const;

    //! return the display name
    QString displayName() const;

    //! return the documentation string associated with the USD attribute.
    QString documentation() const;

    //! return the name token of the USD attribute
    PXR_NS::TfToken name() const;

    //! return the attribute path
    QString path() const;

    //! return the USD prim that owns this attribute.
    PXR_NS::UsdPrim usdPrim() const;

    //! return the underlying USD attribute.
    PXR_NS::UsdAttribute usdAttribute() const;

    //! return type name.
    PXR_NS::SdfValueTypeName usdAttributeType() const;

private:
    PXR_NS::UsdAttribute m_usdAttr;
};

} // namespace TINKERUSD_NS