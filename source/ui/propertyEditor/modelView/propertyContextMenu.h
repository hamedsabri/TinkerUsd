#pragma once

#include <QtCore/QModelIndex>
#include <QtWidgets/QMenu>

namespace TINKERUSD_NS
{

class PropertyTreeView;

/**
 * @class PropertyContextMenu
 * @brief A context menu for property items in the PropertyTreeView.
 */
class PropertyContextMenu : public QMenu
{
    Q_OBJECT
public:
    PropertyContextMenu(PropertyTreeView* treeView, const QModelIndex& index, QWidget* parent = nullptr);

    virtual ~PropertyContextMenu() = default;

private slots:
    // resets the selected property to its default value.
    void resetToDefault(const QModelIndex& index);

    // copies the value of the selected property to the clipboard.
    void copyPropertyValue(const QModelIndex& index);

    // copies the path of the selected property to the clipboard.
    void copyPropertyPath(const QModelIndex& index);

    // copies the type of the selected property to the clipboard.
    void copyPropertyType(const QModelIndex& index);

private:
    PropertyTreeView* m_treeView;
};

} // namespace TINKERUSD_NS