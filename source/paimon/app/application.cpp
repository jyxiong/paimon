#include "paimon/app/application.h"

#include "paimon/app/event/event.h"
#include "paimon/app/window.h"
#include "paimon/app/event/application_event.h"

namespace paimon {

Application::Application(const ApplicationConfig& config) : m_running(false) {
  // Create window
  m_window = Window::create(config.windowConfig);

  // Set event callback
  m_window->setEventCallback([this](Event& event) {
    onEvent(event);
  });

  m_imguiLayer = static_cast<ImGuiLayer*>(
      pushLayer(std::make_unique<ImGuiLayer>("ImGuiLayer", *m_window)));

  m_running = true;
}

Layer* Application::pushLayer(std::unique_ptr<Layer> layer) {
  layer->onAttach();
  m_layers.push_back(std::move(layer));
  return m_layers.back().get();
}

void Application::run() {
  while (m_running) {

    // Poll events
    m_window->pollEvents();

    // Update layers
    for (auto &layer : m_layers) {
      layer->onUpdate();
    }

    // Render layers
    m_imguiLayer->begin();
    for (auto &layer : m_layers) {
      layer->onImGuiRender();
    }
    m_imguiLayer->end();

    // Swap buffers
    m_window->swapBuffers();
  }
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