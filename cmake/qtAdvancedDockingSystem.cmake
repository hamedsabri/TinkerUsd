include(FetchContent)

set(FETCHCONTENT_QUIET ON)

set(QT_DIR ${QT_LOCATION})
set(BUILD_EXAMPLES OFF CACHE INTERNAL "Turn off examples!")
set(BUILD_STATIC ON CACHE INTERNAL "Build Statically!")

FetchContent_Declare(
    qtads
    GIT_REPOSITORY https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System.git
    GIT_TAG master
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(qtads)