#pragma once

#include <memory>
#include <string>

#include "paimon/platform/context.h"

namespace paimon {
struct WindowConfig {
  std::string title;
  ContextFormat format;
  uint32_t width, height;
  bool resizable;
  bool visible;
  bool fullscreen;
  bool vsync;
  bool headless;
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