#include "paimon/app/application.h"

#include "paimon/app/panel/editor_layer.h"
#include "paimon/app/panel/interaction_layer.h"
#include "paimon/config.h"

namespace paimon {

Application* Application::s_instance = nullptr;

Application::Application(const ApplicationConfig& config) {

  s_instance = this;

  // Create window
  m_window = Window::create(config.windowConfig, config.contextFormat);

  m_scene = ecs::Scene::create();

  m_shaderManager.load(PAIMON_SHADER_DIR);

  m_imguiLayer = pushLayer(std::make_unique<ImGuiLayer>());

  pushLayer(std::make_unique<EditorLayer>());

  pushLayer(std::make_unique<InteractionLayer>());

  m_renderer = pushLayer(std::make_unique<Renderer>());
}

void Application::onEvent(Event& event) {
  // Dispatch to layers
  for (auto& layer : m_layers) {
    layer->onEvent(event);
    if (event.handled) break;
  }
}

void Application::run() {
  while (!m_window->shouldClose()) {

    // Poll events
    m_window->pollEvents();

    // Render layers
    m_imguiLayer->begin();
    for (auto &layer : m_layers) {
      layer->onImGuiRender();
    }
    m_imguiLayer->end();

    // Update layers
    for (auto &layer : m_layers) {
      layer->onUpdate();
    }

    // Swap buffers
    m_window->swapBuffers();
  }
}

} // namespace paimon