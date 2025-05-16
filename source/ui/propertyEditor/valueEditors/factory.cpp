#include "factory.h"

#include "common/utils.h"

#include <QDebug>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>

namespace TINKERUSD_NS
{

AbstractPropertyEditor* createNewAttributeEditor(const PXR_NS::UsdAttribute& usdAttr)
{
    auto attrWrapper = UsdAttributeWrapper::create(usdAttr);

    PXR_NS::VtValue value;
    attrWrapper->get(value, PXR_NS::UsdTimeCode::Default());

    const auto typeNameHash = attrWrapper->usdAttributeType().GetHash();

    AbstractPropertyEditor* abstractEditor = nullptr;
    if (typeNameHash == PXR_NS::SdfValueTypeNames->Bool.GetHash())
    {
        abstractEditor = new BooleanEditor(
            attrWrapper->displayName(), value.Get<bool>(), attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Token.GetHash())
    {
        PXR_NS::VtTokenArray             allowedTokens;
        const PXR_NS::UsdPrimDefinition& primDef = attrWrapper->usdPrim().GetPrimDefinition();
        if (primDef.GetPropertyMetadata(
                attrWrapper->name(), PXR_NS::SdfFieldKeys->AllowedTokens, &allowedTokens))
        {
            abstractEditor = new EnumEditor(
                attrWrapper->displayName(),
                EnumData(
                    TINKERUSD_NS::Utils::convert(allowedTokens),
                    QString::fromStdString(value.Get<PXR_NS::TfToken>().GetString())),
                attrWrapper->documentation());
        }
        else
        {
            abstractEditor = new EnumEditor(
                attrWrapper->displayName(),
                EnumData({ QString::fromStdString(value.Get<PXR_NS::TfToken>().GetString()) }, 0),
                attrWrapper->documentation());
        }
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->String.GetHash())
    {
        auto theValue = value.IsEmpty() ? "" : value.Get<std::string>();
        abstractEditor = new StringEditor(
            attrWrapper->displayName(),
            StringData(QString::fromStdString(theValue), true),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Int.GetHash())
    {
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new IntegerEditor(
            attrWrapper->displayName(),
            IntegerGroupSliderData(QPair<int, int>(-8000, 8000), value.Get<int>()),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->UInt.GetHash())
    {
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new UnsignedIntegerEditor(
            attrWrapper->displayName(),
            UnsignedIntegerGroupSliderData(
                QPair<uint32_t, uint32_t>(0, 8000), value.Get<uint32_t>()),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Float.GetHash())
    {
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new FloatEditor(
            attrWrapper->displayName(),
            FloatGroupSliderData(QPair<float, float>(-1000.0f, 1000.0f), value.Get<float>()),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Double.GetHash())
    {
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new DoubleEditor(
            attrWrapper->displayName(),
            DoubleGroupSliderData(QPair<double, double>(-1000.0, 1000.0), value.Get<double>()),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Color3f.GetHash())
    {
        auto colorArray = value.Get<PXR_NS::GfVec3f>();
        abstractEditor = new ColorEditor(
            attrWrapper->displayName(),
            QColor::fromRgbF(colorArray[0], colorArray[1], colorArray[2]),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Color4f.GetHash())
    {
        auto colorArray = value.Get<PXR_NS::GfVec4f>();
        abstractEditor = new ColorEditor(
            attrWrapper->displayName(),
            QColor::fromRgbF(colorArray[0], colorArray[1], colorArray[2], colorArray[3]),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Asset.GetHash())
    {
        abstractEditor = new FileEditor(
            attrWrapper->displayName(),
            FileEditorData(
                QString::fromStdString(value.Get<PXR_NS::SdfAssetPath>().GetAssetPath())),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Float2.GetHash())
    {
        auto vec2f = value.Get<PXR_NS::GfVec2f>();
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new Vector2DEditorFloat(
            attrWrapper->displayName(),
            Vector2dDataFloat(
                QPair<float, float>(-5000.0f, 5000.0f), QVector2D(vec2f[0], vec2f[1])),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Double2.GetHash())
    {
        auto vec2d = value.Get<PXR_NS::GfVec2d>();
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new Vector2DEditorDouble(
            attrWrapper->displayName(),
            Vector2dDataDouble(
                QPair<double, double>(-5000.0, 5000.0), QVector2D(vec2d[0], vec2d[1])),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Float3.GetHash())
    {
        auto vec3f = value.Get<PXR_NS::GfVec3f>();
        // HS TODO: We need to get the appropriate range from Schema
        abstractEditor = new Vector3DEditorFloat(
            attrWrapper->displayName(),
            Vector3dDataFloat(
                QPair<float, float>(-5000.0f, 5000.0f), QVector3D(vec3f[0], vec3f[1], vec3f[2])),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->Double3.GetHash())
    {
        auto vec3d = value.Get<PXR_NS::GfVec3d>();
        abstractEditor = new Vector3DEditorDouble(
            attrWrapper->displayName(),
            Vector3dDataDouble(
                QPair<double, double>(-5000.0, 5000.0), QVector3D(vec3d[0], vec3d[1], vec3d[2])),
            attrWrapper->documentation());
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->BoolArray.GetHash())
    {
        abstractEditor = new ArrayEditor<bool>(
            attrWrapper->displayName(),
            value.Get<PXR_NS::VtArray<bool>>(),
            attrWrapper->documentation(),
            "bool");
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->IntArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<int>>() : PXR_NS::VtArray<int>();
        abstractEditor = new ArrayEditor<int>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "int");
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->FloatArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<float>>() : PXR_NS::VtArray<float>();
        abstractEditor = new ArrayEditor<float>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float");
    }
    else if (
        typeNameHash == PXR_NS::SdfValueTypeNames->Float2Array.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->TexCoord2fArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<GfVec2f>>() : PXR_NS::VtArray<GfVec2f>();
        abstractEditor = new ArrayEditor<GfVec2f>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float2");
    }
    else if (
        typeNameHash == PXR_NS::SdfValueTypeNames->Double2Array.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->TexCoord2dArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<GfVec2d>>() : PXR_NS::VtArray<GfVec2d>();
        abstractEditor = new ArrayEditor<GfVec2d>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "double2");
    }
    else if (
        typeNameHash == PXR_NS::SdfValueTypeNames->Point3fArray.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->Float3Array.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->Color3fArray.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->Vector3fArray.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->Normal3fArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<GfVec3f>>() : PXR_NS::VtArray<GfVec3f>();
        abstractEditor = new ArrayEditor<GfVec3f>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float3");
    }
    else if (
        typeNameHash == PXR_NS::SdfValueTypeNames->Color3dArray.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->Vector3dArray.GetHash()
        || typeNameHash == PXR_NS::SdfValueTypeNames->Normal3dArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<GfVec3d>>() : PXR_NS::VtArray<GfVec3d>();
        abstractEditor = new ArrayEditor<GfVec3d>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "double3");
    }
    else if (typeNameHash == PXR_NS::SdfValueTypeNames->TokenArray.GetHash())
    {
        auto theValue
            = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<TfToken>>() : PXR_NS::VtArray<TfToken>();
        abstractEditor = new ArrayEditor<TfToken>(
            attrWrapper->displayName(), theValue, attrWrapper->documentation(), "token");
    }

    // If no suitable editor was found, create a read-only string editor
    if (!abstractEditor)
    {
        const auto cppTypeName = attrWrapper->usdAttributeType().GetCPPTypeName();
        return new StringEditor(
            attrWrapper->displayName(),
            StringData(
                QString::fromStdString(cppTypeName + " editor is not implemented yet!"), false),
            attrWrapper->documentation());
    }

    abstractEditor->setAttributeWrapper(std::move(attrWrapper));

    return abstractEditor;
}

VariantSetEditor* createVariantSetEditor(const PXR_NS::UsdVariantSet& variantSet)
{
    QStringList variantList;
    for (const auto& variant : variantSet.GetVariantNames())
    {
        variantList << QString::fromStdString(variant);
    }

    return new VariantSetEditor(
        QString::fromStdString(variantSet.GetName()),
        EnumData(variantList, QString::fromStdString(variantSet.GetVariantSelection())),
        variantSet,
        "Variant Set");
}

} // namespace TINKERUSD_NS