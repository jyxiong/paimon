add_library(paimon STATIC
    component/Component.cpp
    component/Component.h
    component/GameObject.cpp
    component/GameObject.h
    component/Transform.cpp
    component/Transform.h

    control/Input.cpp
    control/Input.h
    control/KeyCode.h

    renderer/Camera.cpp
    renderer/Camera.h
    renderer/Material.cpp
    renderer/Material.h
    renderer/MeshFilter.cpp
    renderer/MeshFilter.h
    renderer/MeshRenderer.cpp
    renderer/MeshRenderer.h
    renderer/Shader.cpp
    renderer/Shader.h
    renderer/Texture2D.cpp
    renderer/Texture2D.h

    utils/Application.cpp
    utils/Application.h)

target_precompile_headers(paimon PUBLIC
    pch.h
    )

target_include_directories(paimon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(paimon PUBLIC
    assimp
    glad
    glfw
    glm
    rapidxml
    rttr_core_lib
    stb
    )
