#include "paimon/rendering/renderer.h"

#include <imgui.h>

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"

using namespace paimon;

Renderer::Renderer()
    : Layer("Renderer"), m_renderContext(std::make_unique<RenderContext>()),
      m_color_pass(*m_renderContext), m_final_pass(*m_renderContext) {}

void Renderer::onAttach() {
  // Initialization if needed
}

void Renderer::onUpdate() {
  if (m_resolution.x == 0 || m_resolution.y == 0) {
    return; // Skip rendering if resolution is zero
  }

  auto &scene = Application::getInstance().getScene();

  // First Pass: Render to FBO
  m_color_pass.draw(*m_renderContext, m_resolution, scene);

  // Second Pass: Render FBO texture to screen (optional, for debugging)
  // m_final_pass.draw(*m_renderContext, *m_color_pass.getColorTexture(),
  // m_resolution);
}

void Renderer::onEvent(Event &event) {
  EventDispatcher dispatcher(event);
  dispatcher.dispatch<ViewportResizeEvent>([this](ViewportResizeEvent &e) {
    m_resolution = glm::ivec2(e.getWidth(), e.getHeight());
    return false;
  });
}

void Renderer::onImGuiRender() {}
