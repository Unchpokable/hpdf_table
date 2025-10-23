cmake_minimum_required(VERSION 3.15)

# Platform-specific settings
if(WIN32)
    # On Windows, use vcpkg for dependencies
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
        file(TO_NATIVE_PATH "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" Cmake_toolchain_native)
        set(CMAKE_TOOLCHAIN_FILE "${Cmake_toolchain_native}" CACHE FILEPATH "")
        message("${CMAKE_TOOLCHAIN_FILE}")
        message("${VCPKG_TARGET_TRIPLET}")
    endif()

    # Windows-specific definitions
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(_WIN32)
else()
    # Linux/Unix - use system libraries
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
endif()

project(hpdftbl VERSION 1.5.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build options
option(BUILD_EXAMPLES "Build example programs" ON)
option(ENABLE_JSON_SUPPORT "Enable JSON serialization support (requires jansson)" ON)

# Find required dependencies
find_package(unofficial-libharu CONFIG)
if(NOT unofficial-libharu_FOUND)
    # Try alternative package name
    find_package(libharu CONFIG)
    if(NOT libharu_FOUND)
        # Try pkg-config as fallback
        find_package(PkgConfig)
        if(PkgConfig_FOUND)
            pkg_check_modules(LIBHARU REQUIRED libhpdf)
        else()
            message(FATAL_ERROR "libharu not found. Please install libharu or set up vcpkg.")
        endif()
    endif()
endif()

find_package(Iconv REQUIRED)

# Optional: JSON support via jansson
if(ENABLE_JSON_SUPPORT)
    find_package(jansson CONFIG)
    if(NOT jansson_FOUND)
        find_package(PkgConfig)
        if(PkgConfig_FOUND)
            pkg_check_modules(JANSSON jansson)
        endif()
    endif()

    if(jansson_FOUND OR JANSSON_FOUND)
        add_compile_definitions(HAVE_LIBJANSSON=1)
        set(HAVE_JANSSON TRUE)
    else()
        message(WARNING "jansson not found. JSON serialization will be disabled.")
        set(HAVE_JANSSON FALSE)
    endif()
else()
    set(HAVE_JANSSON FALSE)
endif()

# Generate config.h
configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)

# Add include directory for generated config.h
include_directories(${CMAKE_BINARY_DIR})

# Add subdirectories
add_subdirectory(src)

# Print configuration summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "hpdftbl Configuration Summary")
message(STATUS "========================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "JSON support: ${HAVE_JANSSON}")
message(STATUS "========================================")
message(STATUS "")
