#pragma once

#include "valueEditors/abstractPropertyEditor.h"

#include <optional>
#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/token.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/schema.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QString>
#include <QVariant>

#include <string_view>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS::Utils
{
PXR_NS::UsdPrim getUsdPrimFromSelection();

std::pair<PXR_NS::UsdStageRefPtr, PXR_NS::UsdPrim>
getUsdPrimFromFile(const std::string& filepath, const std::string& primPath);

QStringList convert(const VtTokenArray& tokenArray);

template <typename Container, typename Element>
std::optional<int> findTokenIndex(const Container& container, const Element& element)
{
    static_assert(
        std::is_same_v<typename Container::value_type, Element>,
        "Element type must match the container's element type.");

    if (auto it = std::find(container.cbegin(), container.cend(), element); it != container.cend())
    {
        return std::distance(container.cbegin(), it);
    }

    return std::nullopt;
}

std::optional<std::string_view>
removePrefix(std::string_view attributeName, std::string_view prefix);

TINKERUSD_NS::AbstractPropertyEditor* getEditorFromIndex(const QModelIndex& index);

// This function takes an index from the proxy model and maps it back to the
// corresponding index in the source model.
QModelIndex mapToSourceIndex(QSortFilterProxyModel* proxyModel, const QModelIndex& proxyIndex);

// This function takes an index from the source model and maps it back to the
// corresponding index in the proxy model.
QModelIndex mapFromSourceIndex(QSortFilterProxyModel* proxyModel, const QModelIndex& sourceIndex);

// helper function to return all the value type names.
std::vector<PXR_NS::SdfValueTypeName> getAllValueTypeNames();
} // namespace TINKERUSD_NS::Utils