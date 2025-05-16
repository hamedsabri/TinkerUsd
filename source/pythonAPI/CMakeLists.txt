project(tinkerUsdAPI)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
    PRIVATE
        global.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        tf
        usd
        sdf
        usdGeom
        TinkerUsd
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${USD_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/include
)

compile_config(${PROJECT_NAME})

set(LIBRARY_INSTALL_PATH
    ${CMAKE_INSTALL_PREFIX}/lib
)

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY
    DESTINATION ${LIBRARY_INSTALL_PATH}
    ARCHIVE
    DESTINATION ${LIBRARY_INSTALL_PATH}
    RUNTIME
    DESTINATION ${LIBRARY_INSTALL_PATH}
)

install(FILES __init__.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python/${PROJECT_NAME})

add_subdirectory(python)