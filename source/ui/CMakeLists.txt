# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        mainMenubar.cpp
        mainWindow.cpp
        viewportOpenGLWidget.cpp
        undoManager.cpp
)

add_subdirectory(composition)
add_subdirectory(outliner)
add_subdirectory(scriptEditor)
add_subdirectory(propertyEditor)

