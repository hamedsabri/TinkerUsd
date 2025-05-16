set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES ENABLE_EXPORTS 1)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        main.cpp
)

# -----------------------------------------------------------------------------
# compiler configuration
# -----------------------------------------------------------------------------
compile_config(${TARGET_NAME})

target_compile_definitions(${TARGET_NAME}
    PRIVATE
        $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:TBB_USE_DEBUG>
)

# -----------------------------------------------------------------------------
# include directories
# -----------------------------------------------------------------------------
target_include_directories(${TARGET_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${USD_INCLUDE_DIR}
)

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_libraries(${TARGET_NAME}
    PRIVATE
        usd
        usdGeom
        usdImaging
        usdImagingGL
        gf
        tf
        sdf
        vt
        glf
        hd
        hdx
        hdSt
        garch
        trace
        arch
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        qtadvanceddocking-qt6
        Python3::Python
)

# -----------------------------------------------------------------------------
# install
# -----------------------------------------------------------------------------
install(TARGETS ${TARGET_NAME}
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME} OPTIONAL
    )
endif()

#------------------------------------------------------------------------------
# directories
#------------------------------------------------------------------------------
add_subdirectory(core)
add_subdirectory(render)
add_subdirectory(ui)
add_subdirectory(camera)
add_subdirectory(pythonAPI)