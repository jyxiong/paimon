#include "paimon/app/application.h"

#include "paimon/platform/window.h"

namespace paimon {

Application::Application() : m_running(false) {
  // Create window
  WindowConfig config;
  m_window = Window::create(config);

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

} // namespace paimon