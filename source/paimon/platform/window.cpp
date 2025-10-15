#include "paimon/platform/window.h"

#include "paimon/platform/glfw_window/window.h"

using namespace paimon;

std::unique_ptr<Window> Window::create(const WindowConfig &config) {
  return GlfwWindow::create(config);
}