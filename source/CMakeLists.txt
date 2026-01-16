
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

file(GLOB_RECURSE APP_SOURCES paimon/app/*.cpp)
file(GLOB_RECURSE CORE_SOURCES paimon/core/*.cpp)
file(GLOB_RECURSE OPENGL_SOURCES paimon/opengl/*.cpp)
file(GLOB_RECURSE PLATFORM_SOURCES paimon/platform/*.cpp)
file(GLOB_RECURSE RENDERING_SOURCES paimon/rendering/*.cpp)
file(GLOB_RECURSE UTILITY_SOURCES paimon/utility/*.cpp)

set(TARGET_NAME paimon)

add_library(${TARGET_NAME})

target_sources(${TARGET_NAME} PUBLIC ${APP_SOURCES} ${CORE_SOURCES} ${OPENGL_SOURCES} ${PLATFORM_SOURCES} ${RENDERING_SOURCES} ${UTILITY_SOURCES})

# Generate configuration header with absolute paths
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/paimon/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/paimon/config.h
)

target_include_directories(${TARGET_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_compile_definitions(${TARGET_NAME} PUBLIC GLFW_INCLUDE_NONE)
if(WIN32)
    target_compile_definitions(${TARGET_NAME} PUBLIC PAIMON_PLATFORM_WIN32)
elseif(UNIX)
    if(DEFINED ENV{WAYLAND_DISPLAY})
        target_compile_definitions(${TARGET_NAME} PUBLIC PAIMON_PLATFORM_WAYLAND)
    elseif(DEFINED ENV{DISPLAY})
        target_compile_definitions(${TARGET_NAME} PUBLIC PAIMON_PLATFORM_X11)
    else()
        target_compile_definitions(${TARGET_NAME} PUBLIC PAIMON_PLATFORM_EGL)
    endif()
endif()

target_link_libraries(${TARGET_NAME} PUBLIC EnTT glad glfw glm imgui nfd spdlog stb tinygltf ${OPENGL_LIBRARIES} ${X11_LIBRARIES})

# target_link_options(${TARGET_NAME})
