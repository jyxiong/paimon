#include <iostream>

#include "GLFW/glfw3.h"
#include "glad/gl.h"

#include "paimon/core/base/macro.h"
#include "paimon/platform/context_factory.h"

using namespace paimon;

void create() {
  auto context = ContextFactory::createContext(
      ContextFormat{.versionMajor = 3,
                    .versionMinor = 3,
                    .profile = ContextProfile::Core,
                    .debug = true});

  context->makeCurrent();
  const auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  std::cout << "Created context with version " << versionString << std::endl;
  context->doneCurrent();
}

void getCurrent() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
  const auto window = glfwCreateWindow(320, 240, "", nullptr, nullptr);

  glfwMakeContextCurrent(window);

  auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  std::cout << "Created context with version " << versionString << std::endl;

  auto context = ContextFactory::getCurrentContext();

  context->makeCurrent();
  versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  std::cout << "Created context with version " << versionString << std::endl;

  context->doneCurrent();
  glfwMakeContextCurrent(nullptr);

}

int main() {
  LogSystem::init();

  create();

  getCurrent();

  return 0;
}