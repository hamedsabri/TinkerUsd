#include "propertyproxy.h"

#include "common/utils.h"
#include "valueEditors/abstractPropertyEditor.h"

namespace TINKERUSD_NS
{

PropertyProxy::PropertyProxy(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void PropertyProxy::setSpecialFilter(SpecialFilterKeyword keyword)
{
    m_specialKeyword = keyword;

    // HS: this call is important. we need to tell the proxy model to re-evaluate the
    // items based on the new criteria.
    invalidateFilter();
}

bool PropertyProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);

    auto propertyEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index1);
    if (m_specialKeyword == SpecialFilterKeyword::ModifiedValue)
    {
        if (propertyEditor && !propertyEditor->isDefault())
        {
            return true;
        }
    }
    else if (m_specialKeyword == SpecialFilterKeyword::DefaultValue)
    {
        if (propertyEditor && propertyEditor->isDefault())
        {
            return true;
        }
    }

    // normal filtering by property name
    QString propertyName = sourceModel()->data(index0).toString();

    if (filterRegularExpression().match(propertyName).hasMatch())
    {
        return true;
    }

    // recursively check children
    int childCount = sourceModel()->rowCount(index0);
    for (int i = 0; i < childCount; ++i)
    {
        if (filterAcceptsRow(i, index0))
        {
            return true;
        }
    }

    return false;
}

} // namespace TINKERUSD_NS