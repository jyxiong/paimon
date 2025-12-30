#include "paimon/app/application.h"

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"
#include "paimon/app/event/event.h"
#include "paimon/app/window.h"
#include "paimon/config.h"
#include "paimon/core/ecs/scene.h"

namespace paimon {

Application* Application::s_instance = nullptr;

Application::Application(const ApplicationConfig& config) : m_running(false) {

  s_instance = this;

  // Create window
  m_window = Window::create(config.windowConfig, config.contextFormat);

  // Set event callback
  m_window->setEventCallback([this](Event& event) {
    onEvent(event);
  });

  m_shaderManager.load(PAIMON_SHADER_DIR);

  // Create default empty scene
  m_scene = std::make_unique<ecs::Scene>();

  m_imguiLayer = pushLayer(std::make_unique<ImGuiLayer>("ImGuiLayer"));

  m_running = true;
}

void Application::onEvent(Event& event) {
  // Handle application-level events
  EventDispatcher dispatcher(event);
  dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
    return onWindowClose(e);
  });
  dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
    return onWindowResize(e);
  });

  // Dispatch to layers
  for (auto& layer : m_layers) {
    layer->onEvent(event);
    if (event.handled) break;
  }
}

void Application::run() {
  while (m_running) {

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

bool Application::onWindowClose(const WindowCloseEvent& event) {
  m_running = false;
  return true;
}

bool Application::onWindowResize(const WindowResizeEvent& event) {

  // for (auto& layer : m_layers) {
  //   layer->onResize(event.getWidth(), event.getHeight());
  // }

  return true;
}

} // namespace paimon