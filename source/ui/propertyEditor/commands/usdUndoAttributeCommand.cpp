#include "usdUndoAttributeCommand.h"

namespace TINKERUSD_NS
{

UsdUndoAttributeCommand::UsdUndoAttributeCommand(
    UsdAttributeWrapper* wrapper,
    const PXR_NS::VtValue& newValue,
    PXR_NS::UsdTimeCode time)
    : UndoCommand(wrapper->displayName())
{
    UsdUndoBlock undoBlock(&m_undoableItem);

    wrapper->set(newValue, time);
}

void UsdUndoAttributeCommand::undo()
{
    m_undoableItem.undo();

    if (m_refreshCallback) {
      m_refreshCallback();  
    } 
}

void UsdUndoAttributeCommand::redo()
{
    m_undoableItem.redo();

    if (m_refreshCallback) {
      m_refreshCallback();  
    }
}

} // namespace TINKERUSD_NS