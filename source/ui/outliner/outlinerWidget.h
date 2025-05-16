#pragma once


#include <pxr/usd/usd/stage.h>

#include <QString>
#include <QLineEdit>
#include <QWidget>

namespace TINKERUSD_NS
{

class UsdOutlinerView;
class UsdDocument;
class OutlinerWidget : public QWidget
{
    Q_OBJECT
public:
    OutlinerWidget(UsdDocument* document, QWidget* parent = nullptr);
    virtual ~OutlinerWidget() = default;

private:
    void onCreateUI();

private slots:
    void onStageOpened(const QString& filePath);

private:
    UsdDocument*     m_usdDocument;
    UsdOutlinerView* m_treeView;
    QLineEdit*       m_searchBox;
};

} // namespace TINKERUSD_NS