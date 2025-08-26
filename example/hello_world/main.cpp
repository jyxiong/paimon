#include "paimon/core/base/macro.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

using namespace paimon;

namespace {
auto g_size = glm::ivec2{};
}

void error(int errnum, const char *errmsg) {
  LOG_ERROR("GLFW error {}: {}", errnum, errmsg);
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
  g_size = glm::ivec2{width, height};
}

void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action,
                  int /*modes*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    glfwSetWindowShouldClose(window, true);
}

void glfwError(int error_code, const char *description) {
  LOG_INFO("GLFW error {}: {}", error_code, description);
}

int main() {
  LogSystem::init();

  if (!glfwInit()) {
    LOG_ERROR("GLFW initialization failed. Terminate execution.");

    return 1;
  }

  glfwSetErrorCallback(error);

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  // Create a context and, if valid, make it current
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "globjects Texture", nullptr, nullptr);
  if (window == nullptr) {
    LOG_ERROR("GLFW window creation failed. Terminate execution.");

    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    LOG_ERROR("Failed to initialize OpenGL context");

    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Terminates GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();

  return 0;
}