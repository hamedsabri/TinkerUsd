# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME}
    PRIVATE
        searchBar.cpp
        propertyWidget.cpp
)

add_subdirectory(common)
add_subdirectory(customWidgets)
add_subdirectory(modelView)
add_subdirectory(valueEditors)