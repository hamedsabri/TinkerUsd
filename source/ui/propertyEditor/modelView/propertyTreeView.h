#pragma once

#include <QtWidgets/QTreeView>

namespace TINKERUSD_NS
{

class PropertyModel;
class PropertyDelegate;
class PropertyProxy;

/*
 * @class PropertyTreeView
 * @brief This class implements the Qt TreeView for property editor.
 */
class PropertyTreeView : public QTreeView
{
    Q_OBJECT
public:
    PropertyTreeView(QWidget* parent);

    virtual ~PropertyTreeView() = default;

    // returns a pointer to the PropertyModel
    PropertyModel* getModel() const;

    // returns a pointer to the PropertyProxy
    PropertyProxy* getProxyModel() const;

    // expands all groups in the tree view.
    void expandAllGroups();

    // expands a specific group by name.
    void expandGroup(const QString& groupName);

    // collapses all groups in the tree view.
    void collapseAllGroups();

    // collapses a specific group by name.
    void collapseGroup(const QString& groupName);

    // reapplies persistent editors for items in the tree view.
    void reapplyPersistentEditors();

protected:
    // handles the context menu event to show a custom context menu.
    void contextMenuEvent(QContextMenuEvent* event) override;

    // handles the paint event for custom rendering of the tree view.
    void paintEvent(QPaintEvent* event) override;

private:
    PropertyModel*    m_model;
    PropertyDelegate* m_delegate;
    PropertyProxy*    m_proxyModel;
};

} // namespace TINKERUSD_NS