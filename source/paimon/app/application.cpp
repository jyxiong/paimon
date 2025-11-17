#include "paimon/app/application.h"

#include "paimon/app/event/event.h"
#include "paimon/app/window.h"
#include "paimon/app/event/application_event.h"

namespace paimon {

Application& Application::getInstance() {
  static Application instance;
  return instance;
}

Application::Application() : m_running(false) {
  // Create window
  WindowConfig config;
  m_window = Window::create(config);

  // Set event callback
  m_window->setEventCallback([this](Event& event) {
    onEvent(event);
  });

  m_running = true;
}

Application::~Application() {}

void Application::pushLayer(std::unique_ptr<Layer> layer) {
  layer->onAttach();
  m_layers.push_back(std::move(layer));
}

void Application::run() {
  while (m_running) {

    // Poll events
    m_window->pollEvents();

    // Update layers
    for (auto &layer : m_layers) {
      layer->onUpdate();
    }

    // // Render layers
    // for (auto &layer : m_layers) {
    //   layer->onImGuiRender();
    // }

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
  // TODO: Handle window resize (e.g., update viewport)
  // For example: glViewport(0, 0, event.getWidth(), event.getHeight());
  return false;
}

} // namespace paimon