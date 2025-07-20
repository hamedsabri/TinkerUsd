#include "propertyItem.h"

#include "valueEditors/abstractPropertyEditor.h"

namespace TINKERUSD_NS
{

PropertyItem::PropertyItem(const QString& name, const QVariant& value, AbstractPropertyEditor* editor)
    : QStandardItem(name)
    , m_abstractPropEditor(editor)
{
    setData(value, Qt::EditRole);
    setData(QVariant::fromValue(static_cast<void*>(editor)), Qt::UserRole);
}

AbstractPropertyEditor* PropertyItem::editor() const
{
    return m_abstractPropEditor;
}

} // namespace TINKERUSD_NS
