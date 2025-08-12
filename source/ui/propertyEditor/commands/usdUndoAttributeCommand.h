#include "modelview/usdAttributeWrapper.h"
#include "ui/undoManager.h"

#include <QUndoCommand>
#include <TinkerUsd/undo/usdUndoBlock.h>
#include <TinkerUsd/undo/usdUndoableItem.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS
{

class UsdUndoAttributeCommand : public UndoCommand
{
public:
    UsdUndoAttributeCommand(
        UsdAttributeWrapper*   wrapper,
        const PXR_NS::VtValue& newValue,
        PXR_NS::UsdTimeCode    time);

    void undo() override;
    void redo() override;

    void setRefreshCallback(std::function<void()> cb) { m_refreshCallback = std::move(cb); }

private:
    UsdUndoableItem m_undoableItem;

    std::function<void()> m_refreshCallback;
};

} // namespace TINKERUSD_NS