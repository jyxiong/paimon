#include "paimon/app/panel/viewport_panel.h"

#include <glad/gl.h>
#include <imgui.h>

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"
#include "paimon/opengl/texture.h"

namespace paimon {

ViewportPanel::ViewportPanel() = default;

ViewportPanel::~ViewportPanel() = default;

void ViewportPanel::onImGuiRender() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Viewport");
  
  // Get available content region
  ImVec2 viewportSize = ImGui::GetContentRegionAvail();
  
  // Check if viewport size changed
  glm::ivec2 newSize = glm::ivec2(viewportSize.x, viewportSize.y);
  if (newSize.x > 0 && newSize.y > 0 && newSize != m_viewportSize) {
    m_viewportSize = newSize;
    
    // Dispatch viewport resize event
    ViewportResizeEvent event(newSize.x, newSize.y);
    Application::getInstance().onEvent(event);
  }
  
  // Display the texture if available
  if (m_texture) {
    // Get texture ID (OpenGL texture handle)
    GLuint textureId = m_texture->get_name();
    
    // Display the texture using ImGui::Image
    // Note: ImGui uses bottom-left as origin, OpenGL uses top-left
    // So we need to flip the UV coordinates
    ImGui::Image(
      (ImTextureID)(intptr_t)textureId,
      viewportSize,
      ImVec2(0, 1),  // UV top-left (flipped)
      ImVec2(1, 0)   // UV bottom-right (flipped)
    );
  } else {
    // Show placeholder when no texture
    ImGui::Text("No viewport texture available");
  }
  
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace paimon
