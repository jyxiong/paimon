#include "renderer.h"

#include "paimon/app/application.h"
#include "paimon/app/panel/viewport_panel.h"

using namespace paimon;

Renderer::Renderer()
    : Layer("Renderer"), 
      m_renderContext(std::make_unique<RenderContext>()),
      m_color_pass(*m_renderContext), m_final_pass(*m_renderContext) {}

void Renderer::onUpdate() {
  auto* scene = Application::getInstance().getScene();
  if (!scene) return;

  // First Pass: Render to FBO
  m_color_pass.draw(*m_renderContext, m_resolution, *scene);
  
  // Set the rendered texture to viewport
  if (m_viewportPanel) {
    m_viewportPanel->setTexture(m_color_pass.getColorTexture());
  }

  // Second Pass: Render FBO texture to screen (optional, for debugging)
  // m_final_pass.draw(*m_renderContext, *m_color_pass.getColorTexture(), m_resolution);
}

void Renderer::onEvent(Event &event) {
  EventDispatcher dispatcher(event);
  dispatcher.dispatch<ViewportResizeEvent>([this](ViewportResizeEvent& e) {
    return onViewportResize(e);
  });
}

bool Renderer::onViewportResize(const ViewportResizeEvent& event) {
  m_resolution = glm::ivec2(event.getWidth(), event.getHeight());
  return false; // Allow other layers to handle this event too
}
