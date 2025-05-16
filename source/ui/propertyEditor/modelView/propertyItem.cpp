#include "propertyItem.h"

#include "valueEditors/abstractPropertyEditor.h"

namespace TINKERUSD_NS
{

PropertyItem::PropertyItem(
    const QString&          name,
    const QVariant&         value,
    AbstractPropertyEditor* editor)
    : QStandardItem(name)
    , m_editor(editor)
{
    // store the value associated with the property used for display and editing in the UI.
    setData(value, Qt::EditRole);

    // stores a pointer to the AbstractPropertyEditor associated with the item
    setData(QVariant::fromValue(static_cast<void*>(editor)), Qt::UserRole);
}

AbstractPropertyEditor* PropertyItem::editor() const { return m_editor; }

} // namespace TINKERUSD_NS
