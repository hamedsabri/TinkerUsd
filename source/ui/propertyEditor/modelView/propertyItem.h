#pragma once

#include <QStandardItem>

namespace TINKERUSD_NS
{

class AbstractPropertyEditor;

class PropertyItem : public QStandardItem
{
public:
    PropertyItem(
        const QString&          name,
        const QVariant&         value,
        AbstractPropertyEditor* editor = nullptr);
    virtual ~PropertyItem() = default;

    AbstractPropertyEditor* editor() const;

private:
    AbstractPropertyEditor* m_editor;
};

} // namespace TINKERUSD_NS
