#pragma once

#include <memory>
#include <vector>

#include "paimon/app/event/application_event.h"
#include "paimon/app/event/event.h"
#include "paimon/app/imgui/imgui_layer.h"
#include "paimon/app/layer.h"
#include "paimon/app/window.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/platform/context.h"
#include "paimon/rendering/shader_manager.h"

namespace paimon {

struct ApplicationConfig {
  WindowConfig windowConfig;
  ContextFormat contextFormat;
};

class Application {
public:
  static Application& getInstance() { return *s_instance; }

  Application(const ApplicationConfig& config = {});
  virtual ~Application() = default;

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  template <class T>
    requires std::is_base_of<Layer, T>::value
  T *pushLayer(std::unique_ptr<T> layer) {
    layer->onAttach();
    m_layers.push_back(std::move(layer));
    return static_cast<T *>(m_layers.back().get());
  }

  void onEvent(Event& event);

  void run();

  Window* getWindow() const { return m_window.get(); }

  ShaderManager& getShaderManager() { return m_shaderManager; }
  const ShaderManager& getShaderManager() const { return m_shaderManager; }

  // Scene management
  ecs::Scene* getScene() const { return m_scene.get(); }

private:
  bool onWindowClose(const WindowCloseEvent& event);
  bool onWindowResize(const WindowResizeEvent& event);

private:
  static Application* s_instance;

  std::unique_ptr<Window> m_window;

  ShaderManager m_shaderManager;

  std::vector<std::unique_ptr<Layer>> m_layers;
  ImGuiLayer *m_imguiLayer;

  std::unique_ptr<ecs::Scene> m_scene;

  bool m_running;
};

} // namespace paimon