#include "paimon/platform/glfw/platform.h"

#include "GLFW/glfw3.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

GLFWPlatform &GLFWPlatform::instance() {
  static GLFWPlatform instance;
  return instance;
}

GLFWPlatform::~GLFWPlatform() { glfwTerminate(); }

GLFWPlatform::GLFWPlatform() {
  if (!glfwInit()) {
    LOG_ERROR("glfwInit failed");
    return;
  }

  glfwSetErrorCallback([](int error, const char* description) {
    LOG_ERROR("GLFW error %d: %s", error, description);
  });
}