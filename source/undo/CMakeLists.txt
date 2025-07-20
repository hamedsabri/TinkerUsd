# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        usdUndoStateDelegate.cpp
        usdUndoableItem.cpp
        usdUndoBlock.cpp
        usdUndoManager.cpp
)

set(HEADERS
    usdUndoBlock.h
    usdUndoManager.h
    usdUndoStateDelegate.h
    usdUndoableItem.h
)

promoteHeaderList(HEADERS ${HEADERS} BASEDIR ${PROJECT_NAME} SUBDIR undo)