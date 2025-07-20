#pragma once

#include <QLineEdit>
#include <QTreeView>
#include <memory>
#include <pxr/usd/usd/stage.h>

namespace TINKERUSD_NS
{
class UsdOutlinerModel;
class UsdOutlinerFilterProxyModel;

class UsdOutlinerView : public QTreeView
{
    Q_OBJECT
public:
    UsdOutlinerView(QWidget* parent = nullptr);
    virtual ~UsdOutlinerView() = default;

    void setStage(const PXR_NS::UsdStageRefPtr& stage);
    QLineEdit* searchLineEdit() const { return m_searchLineEdit; }

    void focusPrim(const PXR_NS::UsdPrim& prim);

private slots:
    void onSelectionChanged(const QItemSelection& selected);
    void onSearchTextChanged(const QString& text);

private:
    UsdOutlinerModel*            m_model;
    UsdOutlinerFilterProxyModel* m_proxyModel;
    QLineEdit*                   m_searchLineEdit;
};

} // namespace TINKERUSD_NS