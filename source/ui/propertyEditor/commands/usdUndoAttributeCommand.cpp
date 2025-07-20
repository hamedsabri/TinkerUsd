#include "usdUndoAttributeCommand.h"

namespace TINKERUSD_NS 
{

UsdUndoAttributeCommand::UsdUndoAttributeCommand(UsdAttributeWrapper* wrapper, 
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
}

void UsdUndoAttributeCommand::redo() 
{
    m_undoableItem.redo();
}

} // namespace TINKERUSD_NS