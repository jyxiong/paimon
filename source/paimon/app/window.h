#pragma once

#include <memory>
#include <string>
#include <utility>

#include <GLFW/glfw3.h>

#include "paimon/app/key_code.h"
#include "paimon/app/mouse_code.h"
#include "paimon/platform/context.h"

namespace paimon {

struct WindowConfig {
  std::string title = "Paimon Window";
  int32_t width = 2048;
  int32_t height = 1280;
  bool resizable = true;
  bool visible = true;
  bool fullscreen = false;
  bool vsync = true;
  bool headless = false;
};

class Window {
public:
  Window();

  ~Window();

  void resize(int32_t width, int32_t height);

  void pollEvents();

  void swapBuffers();

  bool shouldClose() const;

  void destroy();

  GLFWwindow* getNativeWindow() const { return m_window; }

  static std::unique_ptr<Window> create(const WindowConfig &config, const ContextFormat &format = ContextFormat());

  // Input functions
  bool isKeyPressed(KeyCode key) const;
  bool isMouseButtonPressed(MouseCode button) const;
  std::pair<double, double> getMousePosition() const;

private:
  GLFWwindow *m_window = nullptr;
};

} // namespace paimon