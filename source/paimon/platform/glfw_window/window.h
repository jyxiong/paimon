#pragma once

#include "paimon/platform/window.h"

#include "GLFW/glfw3.h"

namespace paimon {

class GlfwWindow : public Window {
public:
  GlfwWindow();

  ~GlfwWindow();

  void resize(uint32_t width, uint32_t height) override;

  void pollEvents() override;

  void swapBuffers() override;

  bool shouldClose() const override;

  void destroy() override;

  static std::unique_ptr<Window> create(const WindowConfig &config);

private:
  GLFWwindow *m_window = nullptr;
};

} // namespace paimon