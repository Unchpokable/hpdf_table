# Get all example files
file(GLOB EXAMPLE_SOURCES "*.c")

# Create an executable for each example
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    # Get filename without extension
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)

    # Create executable
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})

    # Link with the library
    target_link_libraries(${EXAMPLE_NAME} PRIVATE hpdftbl)

    # Set output directory
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

list(LENGTH EXAMPLE_SOURCES EXAMPLE_COUNT)
message(STATUS "Configured ${EXAMPLE_COUNT} example programs")
