#include "paimon/platform/glfw/context.h"

#include "glad/gl.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

GlfwContext::GlfwContext(GLFWwindow* window)
  : m_window(window) {
    makeCurrent();

  auto version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    LOG_ERROR("Failed to initialize OpenGL context");
  }
}

GlfwContext::~GlfwContext() {
  doneCurrent();
}

void GlfwContext::makeCurrent() {
  glfwMakeContextCurrent(m_window);
}

void GlfwContext::doneCurrent() {
  glfwMakeContextCurrent(nullptr);
}