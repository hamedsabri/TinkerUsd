#include "factory.h"
#include "common/utils.h"

#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>

namespace TINKERUSD_NS
{

AbstractPropertyEditor* createNewAttributeEditor(const PXR_NS::UsdAttribute& usdAttr)
{
    auto attrWrapper = UsdAttributeWrapper::create(usdAttr);

    PXR_NS::VtValue value;
    attrWrapper->get(value);

    const auto typeName = attrWrapper->usdAttributeType();

    AbstractPropertyEditor* abstractPropEditor = nullptr;

    if (typeName == PXR_NS::SdfValueTypeNames->Bool) {
        abstractPropEditor = new BooleanEditor(attrWrapper->displayName(), value.Get<bool>(), attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Token) {
        PXR_NS::VtTokenArray allowedTokens;
        if (attrWrapper->usdAttribute().GetMetadata(PXR_NS::SdfFieldKeys->AllowedTokens,&allowedTokens)) {
            abstractPropEditor = new EnumEditor(attrWrapper->displayName(), 
                                                EnumData(TINKERUSD_NS::Utils::convert(allowedTokens),
                                                QString::fromStdString(value.Get<PXR_NS::TfToken>().GetString())), 
                                                attrWrapper->documentation());
        }
        else {
            auto theValue = value.IsEmpty() ? PXR_NS::TfToken("") : value.Get<PXR_NS::TfToken>();
            abstractPropEditor = new EnumEditor(attrWrapper->displayName(), 
                                                EnumData({QString::fromStdString(theValue)}, 0), 
                                                attrWrapper->documentation());
        }
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->String) {
        auto theValue = value.IsEmpty() ? "" : value.Get<std::string>();
        abstractPropEditor = new StringEditor(attrWrapper->displayName(), 
                                              StringData(QString::fromStdString(theValue), true),
                                              attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Int) {
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new IntegerEditor(attrWrapper->displayName(), 
                                               IntegerGroupSliderData(QPair<int, int>(-8000, 8000), value.Get<int>()), 
                                               attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->UInt) {
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new UnsignedIntegerEditor(attrWrapper->displayName(), 
                                                       UnsignedIntegerGroupSliderData(QPair<uint32_t, uint32_t>(0, 8000), 
                                                       value.Get<uint32_t>()), 
                                                       attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Float) {
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new FloatEditor(attrWrapper->displayName(), 
                                             FloatGroupSliderData(QPair<float, float>(-1000.0f, 1000.0f), 
                                             value.Get<float>()), 
                                             attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Double) {
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new DoubleEditor(attrWrapper->displayName(),
                                              DoubleGroupSliderData(QPair<double, double>(-1000.0, 1000.0), 
                                              value.Get<double>()), 
                                              attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Color3f) {
        auto colorArray = value.Get<PXR_NS::GfVec3f>();
        abstractPropEditor = new ColorEditor(attrWrapper->displayName(),
                                             QColor::fromRgbF(colorArray[0], colorArray[1], colorArray[2]), 
                                             attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Color4f) {
        auto colorArray = value.Get<PXR_NS::GfVec4f>();
        abstractPropEditor = new ColorEditor(attrWrapper->displayName(), 
                                             QColor::fromRgbF(colorArray[0], colorArray[1], colorArray[2], colorArray[3]), 
                                             attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Asset) {
        abstractPropEditor = new FileEditor(attrWrapper->displayName(), 
                                            FileEditorData(QString::fromStdString(value.Get<PXR_NS::SdfAssetPath>().GetAssetPath())), 
                                            attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Float2) {
        auto vec2f = value.Get<PXR_NS::GfVec2f>();
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new Vector2DEditorFloat(attrWrapper->displayName(), 
                                                     Vector2dDataFloat(QPair<float, float>(-5000.0f, 5000.0f), 
                                                     QVector2D(vec2f[0], vec2f[1])), 
                                                     attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Double2) {
        auto vec2d = value.Get<PXR_NS::GfVec2d>();
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new Vector2DEditorDouble(attrWrapper->displayName(), 
                                                      Vector2dDataDouble(QPair<double, double>(-5000.0, 5000.0), 
                                                      QVector2D(vec2d[0], vec2d[1])), 
                                                      attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Float3) {
        auto vec3f = value.Get<PXR_NS::GfVec3f>();
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new Vector3DEditorFloat(attrWrapper->displayName(), 
                                                     Vector3dDataFloat(QPair<float, float>(-5000.0f, 5000.0f), 
                                                     QVector3D(vec3f[0], vec3f[1], vec3f[2])), 
                                                     attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Double3) {
        auto vec3d = value.Get<PXR_NS::GfVec3d>();
        // HS TODO: hacking the range values for now until they are provided to us
        abstractPropEditor = new Vector3DEditorDouble(attrWrapper->displayName(), 
                                                      Vector3dDataDouble(QPair<double, double>(-5000.0, 5000.0),
                                                      QVector3D(vec3d[0], vec3d[1], vec3d[2])), 
                                                      attrWrapper->documentation());
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->BoolArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<bool>>() : PXR_NS::VtArray<bool>();
        abstractPropEditor = new ArrayEditor<bool>(attrWrapper->displayName(), 
                                                   theValue,
                                                   attrWrapper->documentation(), "bool");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->IntArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<int32_t>>() : PXR_NS::VtArray<int32_t>();
        abstractPropEditor = new ArrayEditor<int>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "int");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->UIntArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<uint32_t>>() : PXR_NS::VtArray<uint32_t>();
        abstractPropEditor = new ArrayEditor<uint32_t>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "uint");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->FloatArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<float>>() : PXR_NS::VtArray<float>();
        abstractPropEditor = new ArrayEditor<float>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->DoubleArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<double>>() : PXR_NS::VtArray<double>();
        abstractPropEditor = new ArrayEditor<double>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "double");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->StringArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<std::string>>() : PXR_NS::VtArray<std::string>();
        abstractPropEditor = new ArrayEditor<std::string>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "string");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Float2Array 
          || typeName == PXR_NS::SdfValueTypeNames->TexCoord2fArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfVec2f>>() : PXR_NS::VtArray<PXR_NS::GfVec2f>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfVec2f>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float2");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Double2Array
          || typeName == PXR_NS::SdfValueTypeNames->TexCoord2dArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfVec2d>>() : PXR_NS::VtArray<PXR_NS::GfVec2d>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfVec2d>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "double2");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Point3fArray 
          || typeName == PXR_NS::SdfValueTypeNames->Float3Array
          || typeName == PXR_NS::SdfValueTypeNames->Color3fArray
          || typeName == PXR_NS::SdfValueTypeNames->Vector3fArray
          || typeName == PXR_NS::SdfValueTypeNames->Normal3fArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfVec3f>>() : PXR_NS::VtArray<PXR_NS::GfVec3f>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfVec3f>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float3");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Color3dArray
          || typeName == PXR_NS::SdfValueTypeNames->Vector3dArray
          || typeName == PXR_NS::SdfValueTypeNames->Normal3dArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfVec3d>>() : PXR_NS::VtArray<PXR_NS::GfVec3d>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfVec3d>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "double3");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Float4Array
          || typeName == PXR_NS::SdfValueTypeNames->Color4fArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfVec4f>>() : PXR_NS::VtArray<PXR_NS::GfVec4f>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfVec4f>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "float4");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Double4Array
        || typeName == PXR_NS::SdfValueTypeNames->Color4dArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfVec4d>>() : PXR_NS::VtArray<PXR_NS::GfVec4d>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfVec4d>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "double4");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Matrix2dArray) {
       auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfMatrix2d>>() : PXR_NS::VtArray<PXR_NS::GfMatrix2d>();
       abstractPropEditor = new ArrayEditor<PXR_NS::GfMatrix2d>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "matrix2d");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Matrix3dArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfMatrix3d>>() : PXR_NS::VtArray<PXR_NS::GfMatrix3d>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfMatrix3d>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "matrix3d");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->Matrix4dArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::GfMatrix4d>>() : PXR_NS::VtArray<PXR_NS::GfMatrix4d>();
        abstractPropEditor = new ArrayEditor<PXR_NS::GfMatrix4d>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "matrix4d");
    }
    else if (typeName == PXR_NS::SdfValueTypeNames->TokenArray) {
        auto theValue = !value.IsEmpty() ? value.Get<PXR_NS::VtArray<PXR_NS::TfToken>>() : PXR_NS::VtArray<PXR_NS::TfToken>();
        abstractPropEditor = new ArrayEditor<PXR_NS::TfToken>(attrWrapper->displayName(), theValue, attrWrapper->documentation(), "token");
    }
    if (!abstractPropEditor) {
        const auto cppTypeName = attrWrapper->usdAttributeType().GetCPPTypeName();
        return new StringEditor(attrWrapper->displayName(), 
                                StringData(QString::fromStdString(cppTypeName + " editor is not implemented yet!"), false),
                                attrWrapper->documentation());
    }

    abstractPropEditor->setAttributeWrapper(std::move(attrWrapper));

    return abstractPropEditor;
}

VariantSetEditor* createVariantSetEditor(const PXR_NS::UsdVariantSet& variantSet)
{
    QStringList variantList;
    for (const auto& variant : variantSet.GetVariantNames()) {
        variantList << QString::fromStdString(variant);
    }

    return new VariantSetEditor(QString::fromStdString(variantSet.GetName()),
                                EnumData(variantList, 
                                         QString::fromStdString(variantSet.GetVariantSelection())),
                                         variantSet, QString());
}

} // namespace TINKERUSD_NS