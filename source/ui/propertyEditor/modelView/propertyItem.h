#pragma once

#include <QtGui/QStandardItem>

namespace TINKERUSD_NS
{

class AbstractPropertyEditor;

/*
 * @class PropertyItem
 * @brief This class implements one property item in the treeview
 */
class PropertyItem : public QStandardItem
{
public:
    PropertyItem(const QString& name, const QVariant& value, AbstractPropertyEditor* editor = nullptr);

    virtual ~PropertyItem() = default;

    // returns a pointer to AbstractPropertyEditor
    AbstractPropertyEditor* editor() const;

private:
    AbstractPropertyEditor* m_abstractPropEditor;
};

} // namespace TINKERUSD_NS
