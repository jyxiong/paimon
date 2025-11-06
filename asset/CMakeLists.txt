# Copy asset folder to build directory
# This makes shaders and other assets available to examples and tests

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/
     DESTINATION ${CMAKE_BINARY_DIR}/asset
     PATTERN "CMakeLists.txt" EXCLUDE)
