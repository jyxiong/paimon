#include "renderer.h"

#include <imgui.h>

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"

using namespace paimon;

Renderer::Renderer()
    : Layer("Renderer"), 
      m_renderContext(std::make_unique<RenderContext>()),
      m_color_pass(*m_renderContext), m_final_pass(*m_renderContext) {}

void Renderer::onAttach() {
  // Initialization if needed
}

void Renderer::onUpdate() {
  auto& scene = Application::getInstance().getScene();

  // First Pass: Render to FBO
  m_color_pass.draw(*m_renderContext, m_resolution, scene);
  
  // Second Pass: Render FBO texture to screen (optional, for debugging)
  // m_final_pass.draw(*m_renderContext, *m_color_pass.getColorTexture(), m_resolution);
}

void Renderer::onEvent(Event &event) {
  EventDispatcher dispatcher(event);
  dispatcher.dispatch<ViewportResizeEvent>([this](ViewportResizeEvent& e) {
    m_resolution = glm::ivec2(e.getWidth(), e.getHeight());
    return false;
  });
}

void Renderer::onImGuiRender() {
  // Get the Viewport window and draw the rendered texture
  if (ImGui::Begin("Viewport")) {
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    
    if (viewportSize.x > 0 && viewportSize.y > 0) {
      GLuint textureId = m_color_pass.getColorTexture()->get_name();
      
      // Display the texture using ImGui::Image
      // Note: OpenGL uses bottom-left origin, so flip UV coordinates
      ImGui::Image(
        (ImTextureID)(intptr_t)textureId,
        viewportSize,
        ImVec2(0, 1),  // UV top-left (flipped)
        ImVec2(1, 0)   // UV bottom-right (flipped)
      );
    }
  }
  ImGui::End();
}
