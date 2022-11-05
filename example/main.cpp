#include <cstdlib>
#include <cstdio>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "component/GameObject.h"
#include "component/Transform.h"
#include "control/Input.h"
#include "control/KeyCode.h"
#include "renderer/Camera.h"
#include "renderer/Material.h"
#include "renderer/MeshFilter.h"
#include "renderer/MeshRenderer.h"
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "utils/application.h"

GLFWwindow *window;

using namespace Paimon;

static void ErrorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Input::RecordKey(static_cast<KeyCode>(key), static_cast<KeyAction>(action));
}

static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    Input::RecordMouseButton(static_cast<MouseButton>(button), static_cast<MouseButtonAction>(action));
}

static void MouseMoveCallback(GLFWwindow *window, double x, double y)
{
    Input::RecordMousePosition(x, y);
}

static void MouseScrollCallback(GLFWwindow *window, double x, double y)
{
    Input::RecordMouseScroll(y);
}

void InitOpengl()
{
    glfwSetErrorCallback(ErrorCallback);

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

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
}

int main()
{
    Application::SetDataPath("../asset");
    InitOpengl();

    // fish soup
    auto go = std::make_shared<GameObject>("fish soup");

    auto transform = go->AddComponent<Transform>();

    auto meshFilter = go->AddComponent<MeshFilter>();
    meshFilter->LoadMesh("model/fishsoup_pot.mesh");

    auto meshRenderer = go->AddComponent<MeshRenderer>();
    auto material = std::make_shared<Material>();
    material->Parse("material/fishsoup_pot.mat");
    meshRenderer->SetMaterial(material);

    // camera
    auto cameraObject = std::make_shared<GameObject>("camera");

    auto cameraTransformComponent = cameraObject->AddComponent<Transform>();
    cameraTransformComponent->SetPosition(glm::vec3(0, 0, 10));

    auto cameraComponent = cameraObject->AddComponent<Camera>();
    cameraComponent->SetDepth(0);

    // camera2
    auto cameraObject2 = std::make_shared<GameObject>("camera2");

    auto cameraTransformComponent2 = cameraObject2->AddComponent<Transform>();
    cameraTransformComponent2->SetPosition(glm::vec3(1, 0, 10));

    auto cameraComponent2 = cameraObject2->AddComponent<Camera>();
    cameraComponent2->SetDepth(1);
    cameraComponent2->SetCullingMask(0x02);
    cameraComponent2->SetClearFlag(GL_DEPTH_BUFFER_BIT);

    CameraManager::SetCamera(cameraComponent);
    CameraManager::SetCamera(cameraComponent2);

    //上一帧的鼠标位置
    auto mousePosition = Input::GetMousePosition();

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        ratio = (float)width / (float)height;

        cameraComponent->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        cameraComponent->SetProjection(glm::radians(60.f), ratio, 1.f, 1000.f);

        cameraComponent2->SetView(glm::vec3(cameraTransformComponent2->GetPosition().x, 0, 0), glm::vec3(0, 1, 0));
        cameraComponent2->SetProjection(glm::radians(60.f), ratio, 1.f, 1000.f);

        // transform model
        if (Input::IsKeyDown(KeyCode::R))
        {
            static float rotateEulerAngle = 0.f;
            rotateEulerAngle += 0.1f;
            auto rotation = transform->GetRotation();
            rotation.y = rotateEulerAngle;
            transform->SetRotation(rotation);
        }

        // transform camera
        if (Input::IsKeyDown(KeyCode::LeftAlt) && Input::IsMouseButtonDown(MouseButton::MouseButtonLeft))
        {
            auto degree = Input::GetMousePosition().x - mousePosition.x;

            auto rotateMat = glm::rotate(glm::mat4(1.0), glm::radians(degree), glm::vec3(0, 1, 0));
            auto position = rotateMat * glm::vec4(cameraTransformComponent->GetPosition(), 1);
            cameraTransformComponent->SetPosition(position);
        }
        mousePosition = Input::GetMousePosition();

        // camera
        cameraTransformComponent->SetPosition(
            cameraTransformComponent->GetPosition() * (10.f - Input::GetMouseScroll()) / 10.f);

        Input::Update();

        CameraManager::Foreach([&]() -> void {
            meshRenderer->Render();
        });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}