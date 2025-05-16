#pragma once

#include <QtWidgets/QWidget>
#include <pxr/usd/usd/stage.h>

namespace TINKERUSD_NS
{

class SearchBar;
class PropertyTreeView;
class UsdDocument;

class PropertyWidget : public QWidget
{
    Q_OBJECT
public:
    PropertyWidget(UsdDocument* document, QWidget* parent = nullptr);
    virtual ~PropertyWidget() = default;

    void onSelectionChanged();

private slots:
    void onStageOpened(const QString& filePath);

private:
    void onCreateUI();

private:
    PropertyTreeView* m_treeView;
    SearchBar*        m_searchBar;
    UsdDocument*      m_usdDocument { nullptr };
};

} // namespace TINKERUSD_NS