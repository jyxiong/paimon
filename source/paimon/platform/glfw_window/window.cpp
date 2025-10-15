#include "paimon/platform/glfw_window/window.h"

#include "GLFW/glfw3.h"
#include "glad/gl.h"
#include "paimon/core/log/log_system.h"

using namespace paimon;

int toGlfw(bool value) { return value ? GLFW_TRUE : GLFW_FALSE; }

int toGlfw(ContextProfile profile) {
  switch (profile) {
  case ContextProfile::Core:
    return GLFW_OPENGL_CORE_PROFILE;
  case ContextProfile::Compatibility:
    return GLFW_OPENGL_COMPAT_PROFILE;
  default:
    return GLFW_OPENGL_ANY_PROFILE;
  }
}

GlfwWindow::GlfwWindow() {
  if (!glfwInit()) {
    LOG_ERROR("glfwInit failed");
    return;
  }

  glfwSetErrorCallback([](int error, const char *description) {
    LOG_ERROR("GLFW error %d: %s", error, description);
  });
}

GlfwWindow::~GlfwWindow() {
  destroy();

  glfwTerminate();
}

void GlfwWindow::resize(uint32_t width, uint32_t height) {
  glfwSetWindowSize(m_window, width, height);
}

void GlfwWindow::pollEvents() { glfwPollEvents(); }

void GlfwWindow::swapBuffers() { glfwSwapBuffers(m_window); }

bool GlfwWindow::shouldClose() const { return glfwWindowShouldClose(m_window); }

void GlfwWindow::destroy() {
  glfwDestroyWindow(m_window);
  m_window = nullptr;
}

std::unique_ptr<Window> GlfwWindow::create(const WindowConfig &config) {
  auto window = std::make_unique<GlfwWindow>();

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_OPENGL_PROFILE, toGlfw(config.format.profile));
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.format.versionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.format.versionMinor);
  glfwWindowHint(GLFW_CONTEXT_DEBUG, toGlfw(config.format.debug));
  glfwWindowHint(GLFW_RESIZABLE, toGlfw(config.resizable));
  glfwWindowHint(GLFW_VISIBLE, toGlfw(config.visible));

  window->m_window = glfwCreateWindow(
      config.width, config.height, config.title.c_str(),
      config.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

  if (!window->m_window) {
    LOG_ERROR("Failed to create GLFW window");
    return nullptr;
  }

  glfwMakeContextCurrent(window->m_window);

  gladLoadGL(glfwGetProcAddress);

  return window;
}
