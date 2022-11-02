#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "renderer/Material.h"
#include "renderer/MeshFilter.h"
#include "renderer/MeshRenderer.h"
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "utils/application.h"

using namespace Paimon;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow *window;

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

    auto meshFilter = std::make_shared<MeshFilter>();
    meshFilter->LoadMesh("model/cube.mesh");

    auto material = std::make_shared<Material>();
    material->Parse("material/cube.mat");

    auto renderer = std::make_shared<MeshRenderer>();
    renderer->SetMeshFilter(meshFilter);
    renderer->SetMaterial(material);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = (float)width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);

        glm::mat4 trans = glm::translate(glm::vec3(0, 0, 0)); //不移动顶点坐标;

        static float rotate_eulerAngle = 0.f;
        rotate_eulerAngle += 1;
        glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(rotate_eulerAngle),
                                                glm::radians(rotate_eulerAngle),
                                                glm::radians(rotate_eulerAngle)); //使用欧拉角旋转;

        glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); //缩放;
        auto model = trans * scale * rotation;
        auto view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        auto projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);
        auto mvp = projection * view * model;
        renderer->SetMVP(mvp);

        renderer->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
