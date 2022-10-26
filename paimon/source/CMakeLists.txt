add_library(paimon STATIC
    renderer/Texture2D.cpp
    renderer/Texture2D.h

    utility/StopWatch.h
    )

target_precompile_headers(paimon PUBLIC
    pch.h
    )

target_include_directories(paimon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(paimon PUBLIC
    assimp
    glad
    glfw
    glm
    stb
)
