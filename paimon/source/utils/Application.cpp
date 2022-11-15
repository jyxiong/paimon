#include "Application.h"

#include "control/Input.h"
#include "component/Component.h"
#include "renderer/Camera.h"
#include "renderer/MeshRenderer.h"
#include "Log.h"

#include "Screen.h"

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

std::filesystem::path Application::s_dataPath;
GLFWwindow *Application::s_window;

void Application::InitOpenGL()
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

    s_window = glfwCreateWindow(960, 640, "Simple example", nullptr, nullptr);
    if (!s_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(s_window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    glfwSetKeyCallback(s_window, KeyCallback);
    glfwSetMouseButtonCallback(s_window, MouseButtonCallback);
    glfwSetScrollCallback(s_window, MouseScrollCallback);
    glfwSetCursorPosCallback(s_window, MouseMoveCallback);
}

void Application::Awake()
{
    Log::Init();
    InitOpenGL();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(s_window))
    {
        Update();
        Render();

        glfwSwapBuffers(s_window);

        glfwPollEvents();
    }

    glfwTerminate();
}

void Application::Update()
{
    UpdateScreenSize();

    EntityManager::ForEachEntity([](const std::shared_ptr<Entity>& entity) -> void {
        entity->ForEachComponent([](const std::shared_ptr<Component> &component) -> void {
            component->Update();
        });
    });

    Input::Update();
}

void Application::UpdateScreenSize()
{
    int width, height;
    glfwGetFramebufferSize(s_window, &width, &height);
    glViewport(0, 0, width, height);

    Screen::SetSize(width, height);
}

void Application::Render()
{
    CameraManager::Foreach([&]() -> void {
        EntityManager::ForEachEntity([](const std::shared_ptr<Entity>& entity) -> void {
            auto meshRenderer = entity->GetComponent<MeshRenderer>();
            if (!meshRenderer)
            {
                return;
            }
            meshRenderer->Render();
        });
    });
}
