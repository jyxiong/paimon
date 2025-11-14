#pragma once

#include <memory>
#include <vector>

#include "paimon/app/event/event.h"
#include "paimon/app/event/application_event.h"
#include "paimon/app/layer.h"
#include "paimon/app/window.h"

namespace paimon {

class Application {
public:
  static Application& getInstance();

  void pushLayer(std::unique_ptr<Layer> layer);

  void run();

  Window* getWindow() const { return m_window.get(); }

  void onEvent(Event& event);

private:

  Application();
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  bool onWindowClose(const WindowCloseEvent& event);
  bool onWindowResize(const WindowResizeEvent& event);

private:

  std::unique_ptr<Window> m_window;

  std::vector<std::unique_ptr<Layer>> m_layers;

  bool m_running;
};

} // namespace paimon