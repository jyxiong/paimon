cmake_minimum_required(VERSION 3.20)
project(Paimon)

# use c++ 17
set(CMAKE_CXX_STANDARD 20)

# add cmake module path to find 3rdparty libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

# add configure
configure_file(configuration/root_directory.h.in configuration/root_directory.h)

# find package
find_package(GFLAGS REQUIRED)
find_package(GLOG REQUIRED)
find_package(GLAD REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)

# find other lib on macos
find_library(COCOA_LIBRARY Cocoa)
find_library(OpenGL_LIBRARY OpenGL)
find_library(IOKit_LIBRARY IOKit)
find_library(CoreVideo_LIBRARY CoreVideo)
set(APPLE_LIBRARIES ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})

# build paimon engine
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(paimon STATIC ${HEADERS} ${SOURCES})
target_include_directories(paimon PUBLIC
        ${GFLAGS_INCLUDE_DIR}
        ${GLOG_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
        ${GLFW3_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        )
target_link_libraries(paimon PUBLIC
        ${GFLAGS_LIBRARY}
        ${GLOG_LIBRARY}
        ${GLAD_LIBRARY}
        ${GLFW3_LIBRARY}
        ${GLM_LIBRARY}
        ${APPLE_LIBRARIES}
        )

# build samples
option(PAIMON_BUILD_EXAMPLE "Build Samples for Paimon" on)
if (PAIMON_BUILD_EXAMPLE)
    add_subdirectory(samples)
endif(PAIMON_BUILD_EXAMPLE)
