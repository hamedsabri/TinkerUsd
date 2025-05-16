#pragma once

#include "layerStack.h"
#include "primComposition.h"

#include <QPointer>
#include <QWidget>

class QMainWindow;

namespace TINKERUSD_NS
{

class UsdDocument;
class CompositionInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    CompositionInspectorWidget(UsdDocument* document, QMainWindow* parent = nullptr);
    virtual ~CompositionInspectorWidget();

    void onSelectionChanged();

private:
    void setupLayout();

private slots:
    void onStageOpened(const QString& filePath);

private:
    QPointer<LayerStack>      m_layerStackWidget;
    QPointer<PrimComposition> m_primCompositionWidget;
    UsdDocument*              m_usdDocument { nullptr };
};

} // namespace TINKERUSD_NS