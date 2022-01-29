project(hello_glog)

set(TARGET ${PROJECT_NAME})
add_executable(${TARGET} hello_glog.cpp)
target_link_libraries(${TARGET} PUBLIC
        ${GFLAGS_LIBRARY}
        ${GLOG_LIBRARY}
        )
target_include_directories(${TARGET} PUBLIC
        ${GFLAGS_INCLUDE_DIR}
        ${GLOG_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/configuration
        )