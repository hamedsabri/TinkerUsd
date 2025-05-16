# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME}
    PRIVATE
        globalSelection.cpp
        usdDocument.cpp
        utils.cpp
)

set(HEADERS
    utils.h
)

promoteHeaderList(HEADERS ${HEADERS} BASEDIR ${PROJECT_NAME} SUBDIR core)