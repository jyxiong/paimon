
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

set(TARGET_NAME paimon)
file(GLOB_RECURSE TARGET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE TARGET_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS})

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_definitions(${TARGET_NAME} PUBLIC GLFW_INCLUDE_NONE)

# target_compile_options(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC glad glfw glm spdlog ${OPENGL_LIBRARIES} ${X11_LIBRARIES})

# target_link_options(${TARGET_NAME})
