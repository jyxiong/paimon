cmake_minimum_required(VERSION 3.20)

project(paimon)

# headless rendering
option(BUILD_WITH_GLFW "Build with GLFW support" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMakeTargets)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_POSTFIX "d")

add_subdirectory(asset)
add_subdirectory(example)
add_subdirectory(external)
add_subdirectory(source)