#include <stdlib.h>
#include <stdio.h>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "component/GameObject.h"
#include "component/Transform.h"
#include "renderer/Material.h"
#include "renderer/MeshFilter.h"
#include "renderer/MeshRenderer.h"
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "utils/application.h"

GLFWwindow *window;

using namespace Paimon;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void InitOpengl()
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(960, 640, "Simple example", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}

int main()
{
    Application::SetDataPath("../asset");
    InitOpengl();

    //创建GameObject
    auto go = std::make_shared<GameObject>("something");

    auto transform = go->AddComponent<Transform>();

    auto meshFilter = go->AddComponent<MeshFilter>();
    meshFilter->LoadMesh("model/fishsoup_pot.mesh");

    auto meshRenderer = go->AddComponent<MeshRenderer>();
    auto material = std::make_shared<Material>();
    material->Parse("material/fishsoup_pot.mat");
    meshRenderer->SetMaterial(material);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = (float)width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);

        static float rotateEulerAngle = 0.f;
        rotateEulerAngle += 0.1f;
        auto rotation = transform->GetRotation();
        rotation.y = rotateEulerAngle;
        transform->SetRotation(rotation);

        auto view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        auto projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);

        meshRenderer->SetView(view);
        meshRenderer->SetProjection(projection);
        meshRenderer->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}