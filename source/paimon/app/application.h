#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "paimon/app/event/application_event.h"
#include "paimon/app/event/event.h"
#include "paimon/app/imgui/imgui_layer.h"
#include "paimon/app/layer.h"
#include "paimon/app/window.h"

namespace paimon {

struct ApplicationConfig {
  WindowConfig windowConfig;
  std::filesystem::path assetPath;
};

class Application {
public:
  Application(const ApplicationConfig& config);
  ~Application() = default;

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  template <class T>
    requires std::is_base_of<Layer, T>::value
  T *pushLayer(std::unique_ptr<T> layer) {
    layer->onAttach();
    m_layers.push_back(std::move(layer));
    return static_cast<T *>(m_layers.back().get());
  }

  void run();

  Window* getWindow() const { return m_window.get(); }

private:
  void onEvent(Event& event);
  bool onWindowClose(const WindowCloseEvent& event);
  bool onWindowResize(const WindowResizeEvent& event);

private:

  std::unique_ptr<Window> m_window;

  std::vector<std::unique_ptr<Layer>> m_layers;
  ImGuiLayer *m_imguiLayer;

  bool m_running;
};

} // namespace paimon