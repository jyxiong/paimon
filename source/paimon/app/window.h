#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <GLFW/glfw3.h>

#include "paimon/app/event/event.h"
#include "paimon/app/key_code.h"
#include "paimon/app/mouse_code.h"
#include "paimon/platform/context.h"

namespace paimon {

struct WindowConfig {
  std::string title = "Paimon Window";
  ContextFormat format;
  int32_t width = 800;
  int32_t height = 600;
  bool resizable = false;
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

  static std::unique_ptr<Window> create(const WindowConfig &config);

  // Input functions
  bool isKeyPressed(KeyCode key) const;
  bool isMouseButtonPressed(MouseCode button) const;
  std::pair<double, double> getMousePosition() const;

  void setEventCallback(const std::function<void(Event&)>& callback) { m_eventCallback = callback; }

  const std::function<void(Event&)>& getEventCallback() const { return m_eventCallback; }

private:
  GLFWwindow *m_window = nullptr;

public:
  std::function<void(Event&)> m_eventCallback;
};

} // namespace paimon