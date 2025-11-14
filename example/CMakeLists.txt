function(copy_example_assets EXAMPLE_NAME)
    # Copy asset folder from example to build directory
    set(EXAMPLE_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}/asset)
    set(BUILD_ASSET_DIR ${CMAKE_BINARY_DIR}/example/${EXAMPLE_NAME}/asset)

    if(EXISTS ${EXAMPLE_ASSET_DIR})
        # Collect all asset files
        file(GLOB_RECURSE ASSET_FILES
            RELATIVE ${EXAMPLE_ASSET_DIR}
            "${EXAMPLE_ASSET_DIR}/*"
        )

        # Copy each asset file
        foreach(ASSET_FILE ${ASSET_FILES})
            configure_file(
                "${EXAMPLE_ASSET_DIR}/${ASSET_FILE}"
                "${BUILD_ASSET_DIR}/${ASSET_FILE}"
                COPYONLY
            )
        endforeach()

        # Print summary
        list(LENGTH ASSET_FILES ASSET_COUNT)
        message(STATUS "Configured ${ASSET_COUNT} asset files for ${EXAMPLE_NAME}")
    endif()
endfunction()

function(add_example FOLDER_NAME)
    set(EXAMPLE_NAME ${FOLDER_NAME})
    file(GLOB_RECURSE SOURCE_FILES ${FOLDER_NAME}/*.cpp ${FOLDER_NAME}/*.h)
    add_executable(${EXAMPLE_NAME} ${SOURCE_FILES})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE paimon)

    # Set output directory to the example's folder
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example/${FOLDER_NAME}
    )

    # Copy assets for this example
    copy_example_assets(${FOLDER_NAME})
endfunction()

add_example(hello_world)
add_example(texture)
add_example(framebuffer)
add_example(query)
add_example(separate_program)
add_example(debug_message)
add_example(state)
add_example(context)
add_example(geometry)
add_example(compute_shader)
add_example(frame_graph)
add_example(process_shader)
add_example(damaged_helmet)
add_example(application)
