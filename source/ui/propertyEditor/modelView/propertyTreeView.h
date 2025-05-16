#pragma once

#include <QtWidgets/QTreeView>

namespace TINKERUSD_NS
{

class PropertyModel;
class PropertyDelegate;
class PropertyProxy;

class PropertyTreeView : public QTreeView
{
    Q_OBJECT
public:
    PropertyTreeView(QWidget* parent);
    virtual ~PropertyTreeView() = default;

    PropertyModel* getModel() const;
    PropertyProxy* getProxyModel() const;

    void expandAllGroups();
    void expandGroup(const QString& groupName);

    void collapseAllGroups();
    void collapseGroup(const QString& groupName);

    void reapplyPersistentEditors();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    PropertyModel*    m_model;
    PropertyDelegate* m_delegate;
    PropertyProxy*    m_proxyModel;
};

} // namespace TINKERUSD_NS
