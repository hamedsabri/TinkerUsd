#include "utils.h"

#include "valueEditors/factory.h"

#include <QVector3D>

namespace TINKERUSD_NS::Utils
{

std::pair<PXR_NS::UsdStageRefPtr, PXR_NS::UsdPrim>
getUsdPrimFromFile(const std::string& filePath, const std::string& primPath)
{
    PXR_NS::UsdStageRefPtr stage = PXR_NS::UsdStage::Open(filePath);

    if (!stage)
    {
        TF_RUNTIME_ERROR("Error opening stage for '%s'", filePath.c_str());
        return { nullptr, PXR_NS::UsdPrim() };
    }

    PXR_NS::UsdPrim prim = stage->GetPrimAtPath(PXR_NS::SdfPath(primPath));
    if (!prim.IsValid())
    {
        return { nullptr, PXR_NS::UsdPrim() };
    }

    return { stage, prim };
}

QStringList convert(const VtTokenArray& tokenArray)
{
    QStringList stringList;
    for (const auto& token : tokenArray)
    {
        stringList.append(QString::fromStdString(token.GetString()));
    }
    return stringList;
}

std::optional<std::string_view> removePrefix(std::string_view attributeName, std::string_view prefix)
{
    if (attributeName.substr(0, prefix.size()) == prefix)
    {
        return attributeName.substr(prefix.size());
    }
    return std::nullopt;
}

AbstractPropertyEditor* getEditorFromIndex(const QModelIndex& index)
{
    return static_cast<AbstractPropertyEditor*>(index.data(Qt::UserRole).value<void*>());
}

QModelIndex mapToSourceIndex(QSortFilterProxyModel* proxyModel, const QModelIndex& proxyIndex)
{
    if (!proxyModel)
    {
        return QModelIndex();
    }

    return proxyModel->mapToSource(proxyIndex);
}

QModelIndex mapFromSourceIndex(QSortFilterProxyModel* proxyModel, const QModelIndex& sourceIndex)
{
    if (!proxyModel)
    {
        return QModelIndex();
    }

    return proxyModel->mapFromSource(sourceIndex);
}

std::vector<PXR_NS::SdfValueTypeName> getAllValueTypeNames()
{
    return PXR_NS::SdfSchema::GetInstance().GetAllTypes();
}

} // namespace TINKERUSD_NS::Utils