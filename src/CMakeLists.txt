# Source files for the library
set(HPDFTBL_SOURCES
    hpdftbl.c
    hpdftbl_callback.c
    hpdftbl_errstr.c
    hpdftbl_grid.c
    hpdftbl_theme.c
    hpdftbl_widget.c
    read_file.c
    xstr.c
)

# Conditionally add JSON support files
if(HAVE_JANSSON)
    list(APPEND HPDFTBL_SOURCES
        hpdftbl_dump.c
        hpdftbl_load.c
    )
endif()

# Header files
set(HPDFTBL_HEADERS
    hpdftbl.h
)

# Build static library
add_library(hpdftbl STATIC ${HPDFTBL_SOURCES})

# Set include directories
target_include_directories(hpdftbl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

# Link dependencies
# Add iconv
if(TARGET Iconv::Iconv)
    target_link_libraries(hpdftbl PUBLIC Iconv::Iconv)
elseif(Iconv_FOUND)
    target_include_directories(hpdftbl PUBLIC ${Iconv_INCLUDE_DIRS})
    target_link_libraries(hpdftbl PUBLIC ${Iconv_LIBRARIES})
endif()

# Add libharu
if(TARGET unofficial::libharu::hpdf)
    target_link_libraries(hpdftbl PUBLIC unofficial::libharu::hpdf)
elseif(TARGET hpdf)
    target_link_libraries(hpdftbl PUBLIC hpdf)
elseif(LIBHARU_FOUND)
    target_include_directories(hpdftbl PUBLIC ${LIBHARU_INCLUDE_DIRS})
    target_link_libraries(hpdftbl PUBLIC ${LIBHARU_LIBRARIES})
endif()

# Add jansson if available
if(HAVE_JANSSON)
    if(TARGET jansson::jansson)
        target_link_libraries(hpdftbl PUBLIC jansson::jansson)
    elseif(JANSSON_FOUND)
        target_include_directories(hpdftbl PUBLIC ${JANSSON_INCLUDE_DIRS})
        target_link_libraries(hpdftbl PUBLIC ${JANSSON_LIBRARIES})
    endif()
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(hpdftbl PUBLIC shlwapi)
else()
    # Linux/Unix math library
    target_link_libraries(hpdftbl PUBLIC m)
endif()

# Set output name
set_target_properties(hpdftbl PROPERTIES
    OUTPUT_NAME hpdftbl
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
