function(promoteHeaderList)
    cmake_parse_arguments(PREFIX
        ""
        "SUBDIR;BASEDIR" # one_value keywords
        "HEADERS" # multi_value keywords
        ${ARGN}
    )

    if (PREFIX_HEADERS)
        set(HEADERFILES ${PREFIX_HEADERS})
    else()
        message(FATAL_ERROR "HEADERS keyword is not specified.")
    endif()

    set(BASEDIR ${CMAKE_BINARY_DIR}/include)
    if (PREFIX_BASEDIR)
        set(BASEDIR ${BASEDIR}/${PREFIX_BASEDIR})
    else()
        set(BASEDIR ${BASEDIR}/mayaUsd)
    endif()

    if (PREFIX_SUBDIR)
        set(BASEDIR ${BASEDIR}/${PREFIX_SUBDIR})
    endif()

    foreach(header ${HEADERFILES})
        set(SRCFILE ${CMAKE_CURRENT_SOURCE_DIR}/${header})
        set(DSTFILE ${BASEDIR}/${header})

        set(CONTENT "#pragma once\n#include \"${SRCFILE}\"\n")

        if (NOT EXISTS ${DSTFILE})
            message(STATUS "promoting: " ${SRCFILE})
            file(WRITE ${DSTFILE} "${CONTENT}")
        else()
            file(READ ${DSTFILE} oldContent)
            if (NOT "${CONTENT}" STREQUAL "${oldContent}")
                message(STATUS "Promoting ${SRCFILE}")
                file(WRITE ${DSTFILE} "${CONTENT}")
            endif()
        endif()
    endforeach()
endfunction()
