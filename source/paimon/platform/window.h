#pragma once

#include <memory>
#include <string>

#include "paimon/platform/context.h"

namespace paimon {
struct WindowConfig {
  std::string title = "Paimon Window";
  ContextFormat format;
  uint32_t width = 800;
  uint32_t height = 600;
  bool resizable = false;
  bool visible = true;
  bool fullscreen = false;
  bool vsync = true;
  bool headless = false;
};

class Window {
public:
  virtual ~Window() = default;

  virtual void resize(uint32_t width, uint32_t height) = 0;

  virtual void pollEvents() = 0;

  virtual void swapBuffers() = 0;

  virtual bool shouldClose() const = 0;

  virtual void destroy() = 0;

  static std::unique_ptr<Window> create(const WindowConfig &config);
};
} // namespace paimon