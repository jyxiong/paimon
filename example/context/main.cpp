#include <iostream>

#include "GLFW/glfw3.h"
#include "glad/gl.h"

#include "paimon/core/base/macro.h"
#include "paimon/platform/context_factory.h"

using namespace paimon;

int main() {
  LogSystem::init();

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  const auto window = glfwCreateWindow(320, 240, "", nullptr, nullptr);

  glfwMakeContextCurrent(window);

  auto context = ContextFactory::getCurrentContext();
  if (context == nullptr || !context->valid()) {
    LOG_ERROR("Failed to get current context");
    return -1;
  }

  glfwMakeContextCurrent(nullptr);

  context->makeCurrent();
  const auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  std::cout << "Created context with version " << versionString << std::endl;

  context->doneCurrent();

  return 0;
}