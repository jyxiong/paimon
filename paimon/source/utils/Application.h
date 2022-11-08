#pragma once

#include <filesystem>
#include <memory>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

namespace Paimon
{

class Application
{
public:
    static const std::filesystem::path &GetDataPath() { return s_dataPath; }
    static void SetDataPath(const std::filesystem::path &path) { s_dataPath = path; };

    static void InitOpengl();

    static void Awake();
    static void Run();

    static void Update();
    static void UpdateScreenSize();

    static void Render();

public:
    static std::filesystem::path s_dataPath;

    static GLFWwindow *s_window;

}; // class Application

} // namespace Paimon
