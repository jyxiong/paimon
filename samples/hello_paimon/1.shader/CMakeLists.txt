cmake_minimum_required(VERSION 3.16)

project(1.shader)

set(TARGET ${PROJECT_NAME})

file(GLOB SOURCE *.h *.cpp)
add_executable(${TARGET} ${SOURCE})

target_link_libraries(${TARGET} PUBLIC
        ${GLAD_LIBRARY}
        ${GLFW3_LIBRARY}
        ${APPLE_LIBRARIES}
        paimon
        )
target_include_directories(${TARGET} PUBLIC
        ${GLAD_INCLUDE_DIR}
        ${GLFW3_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        )

# copy shader files to build directory
file(GLOB SHADERS *.vs *.fs)
foreach(SHADER ${SHADERS})
    file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(SHADER)