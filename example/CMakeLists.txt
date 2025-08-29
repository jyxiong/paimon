function(add_example FOLDER_NAME)
    set(EXAMPLE_NAME ${FOLDER_NAME})
    add_executable(${EXAMPLE_NAME} ${FOLDER_NAME}/main.cpp)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE paimon)
endfunction()

add_example(hello_world)
add_example(texture)