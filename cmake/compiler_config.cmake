#------------------------------------------------------------------------------
# compiler flags/definitions
#------------------------------------------------------------------------------
set(MSVC_FLAGS
    /W3
    /WX
    /permissive
    /Zi
    /Zc:__cplusplus
    /Zc:rvalueCast
    /Zc:inline
    /Zc:inline
    /MP
    /EHsc
    /w35038
    # disable warnings
    /wd4244
    /wd4267
    /wd4273
    /wd4305
    /wd4506
    /wd4996
    /wd4180
    # exporting STL classes
    /wd4251
    # Set some warnings as errors (to make it similar to Linux)
    /we4101
    /we4189
    $<$<BOOL:${ENABLE_ASAN}>:/fsanitize=address>
)

set(MSVC_DEFINITIONS
    # Make sure WinDef.h does not define min and max macros which
    # will conflict with std::min() and std::max().
    NOMINMAX

    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS

    # Needed to prevent Python from adding a define for snprintf
    # since it was added in Visual Studio 2015.
    HAVE_SNPRINTF
)

#------------------------------------------------------------------------------
# compiler configuration
#------------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)

function(compile_config TARGET)
    target_link_options(${TARGET}
        PRIVATE
            $<$<BOOL:${ENABLE_ASAN}>:/fsanitize=address>
    )
    target_compile_features(${TARGET} 
        PRIVATE
            cxx_std_20
    )
    target_compile_options(${TARGET} 
        PRIVATE
            ${MSVC_FLAGS}
    )
    target_compile_definitions(${TARGET} 
        PRIVATE
            ${MSVC_DEFINITIONS}
    )
    target_compile_definitions(${TARGET}
        PRIVATE
            TBB_SUPPRESS_DEPRECATED_MESSAGES
    )
endfunction()