#pragma once

#include <QTableWidget>
#include <pxr/usd/usd/prim.h>

namespace TINKERUSD_NS
{

class LayerStack : public QTableWidget
{
    Q_OBJECT
public:
    LayerStack(QWidget* parent = nullptr);
    virtual ~LayerStack() = default;

    void setTableData(const PXR_NS::UsdPrim& prim);
    void clearAll();

private:
    void setupLayout();
    void showContextMenu(const QPoint& pos);
    void paintEvent(QPaintEvent* event) override;
};

} // namespace TINKERUSD_NS
