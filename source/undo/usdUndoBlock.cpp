//
// Copyright 2020 Autodesk
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "usdUndoBlock.h"
#include "debugCodes.h"

namespace TINKERUSD_NS 
{

uint32_t UsdUndoBlock::_undoBlockDepth { 0 };

UsdUndoBlock::UsdUndoBlock(UsdUndoableItem* undoItem, bool extraEdits)
    : _undoItem(undoItem)
    , _extraEdits(extraEdits)
{
    TfDebug::Enable(UNDOSTACK);

    TF_DEBUG_MSG(UNDOSTACK, "--Opening undo block at depth %i\n", _undoBlockDepth);

    ++_undoBlockDepth;
}

UsdUndoBlock::~UsdUndoBlock()
{
    // decrease the depth
    --_undoBlockDepth;

    if ((nullptr != _undoItem) && (_undoBlockDepth == 0)) {
        // transfer edits
        UsdUndoManagerAccessor::transferEdits(*_undoItem, _extraEdits);

        TF_DEBUG_MSG(UNDOSTACK, "Undoable Item adopted the new edits.\n");
    }

    TF_DEBUG_MSG(UNDOSTACK, "--Closed undo block at depth %i\n", _undoBlockDepth);
}

} // namespace TINKERUSD_NS
