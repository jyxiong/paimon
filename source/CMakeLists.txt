
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

file(GLOB_RECURSE CORE_SOURCES paimon/core/*.cpp)
file(GLOB_RECURSE OPENGL_SOURCES paimon/opengl/*.cpp)
file(GLOB PLATFORM_SOURCES paimon/platform/*.cpp)
message(STATUS ${PLATFORM_SOURCES})

if(BUILD_WITH_GLFW)
    list(APPEND PLATFORM_SOURCES paimon/platform/glfw/context.cpp paimon/platform/glfw/platform.cpp)
    message(STATUS "GLFW enabled: using GLFW")
elseif(WIN32)
    list(APPEND PLATFORM_SOURCES paimon/platform/wgl/context.cpp paimon/platform/wgl/platform.cpp)
    message(STATUS "Windows detected: using WGL")
elseif(UNIX)
    if(DEFINED ENV{WAYLAND_DISPLAY})
        list(APPEND PLATFORM_SOURCES paimon/platform/egl/context.cpp paimon/platform/egl/platform.cpp)
        message(STATUS "Wayland detected: using EGL")
    elseif(DEFINED ENV{DISPLAY})
        list(APPEND PLATFORM_SOURCES paimon/platform/glx/context.cpp paimon/platform/glx/platform.cpp)
        message(STATUS "X11 detected: using GLX")
    else()
        list(APPEND PLATFORM_SOURCES paimon/platform/egl/context.cpp paimon/platform/egl/platform.cpp)
        message(STATUS "Headless detected: using EGL")
    endif()
endif()

set(TARGET_NAME paimon)

add_library(${TARGET_NAME})

target_sources(${TARGET_NAME} PUBLIC ${CORE_SOURCES} ${OPENGL_SOURCES} ${PLATFORM_SOURCES})

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_definitions(${TARGET_NAME} PUBLIC GLFW_INCLUDE_NONE)

# target_compile_options(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC glad glfw glm spdlog ${OPENGL_LIBRARIES} ${X11_LIBRARIES})

# target_link_options(${TARGET_NAME})
