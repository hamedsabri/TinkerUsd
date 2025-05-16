#include "propertyproxy.h"

#include "valueEditors/abstractPropertyEditor.h"

namespace TINKERUSD_NS
{

PropertyProxy::PropertyProxy(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_specialKeyword(SpecialFilterKeyword::None)
{
}

void PropertyProxy::setSpecialFilter(SpecialFilterKeyword keyword)
{
    m_specialKeyword = keyword;
    invalidateFilter();
}

bool PropertyProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);

    AbstractPropertyEditor* property
        = static_cast<AbstractPropertyEditor*>(index1.data(Qt::UserRole).value<void*>());
    if (m_specialKeyword == SpecialFilterKeyword::ModifiedValue)
    {
        if (property && !property->isDefault())
        {
            return true;
        }
    }
    else if (m_specialKeyword == SpecialFilterKeyword::DefaultValue)
    {
        if (property && property->isDefault())
        {
            return true;
        }
    }

    // Check if this is a group item
    if (sourceModel()->hasChildren(index0))
    {
        for (int i = 0; i < sourceModel()->rowCount(index0); ++i)
        {
            if (filterAcceptsRow(i, index0))
            {
                return true; // If any child passes the filter, accept the group item
            }
        }
        return false; // No children passed the filter, so hide the group item
    }

    // Perform filtering based on the standard role (usually Qt::DisplayRole)
    return sourceModel()->data(index0).toString().contains(filterRegularExpression());
}
} // namespace TINKERUSD_NS