# Copy asset folder to build directory
# This makes shaders and other assets available to examples and tests

# Collect all asset files (excluding CMakeLists.txt)
file(GLOB_RECURSE ASSET_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
)

# Filter out CMakeLists.txt
list(FILTER ASSET_FILES EXCLUDE REGEX "CMakeLists.txt$")

# Use configure_file for automatic dependency tracking
# CMake will automatically detect file changes and only copy modified files
foreach(ASSET_FILE ${ASSET_FILES})
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET_FILE}"
        "${CMAKE_BINARY_DIR}/asset/${ASSET_FILE}"
        COPYONLY
    )
endforeach()

# Print summary
list(LENGTH ASSET_FILES ASSET_COUNT)
message(STATUS "Configured ${ASSET_COUNT} asset files for automatic copying")
message(STATUS "Asset destination: ${CMAKE_BINARY_DIR}/asset")
