#pragma once

#include <QPoint>
#include <QTreeWidget>

#include <pxr/usd/usd/prim.h>

namespace TINKERUSD_NS
{

class PrimComposition : public QTreeWidget
{
    Q_OBJECT
public:
    PrimComposition(QWidget* parent = nullptr);
    virtual ~PrimComposition() = default;

    void setTreeData(const PXR_NS::UsdPrim& prim);
    void clearAll();

private:
    void setupLayout();
    void showContextMenu(const QPoint& pos);
    void paintEvent(QPaintEvent* event) override;
};

} // namespace TINKERUSD_NS