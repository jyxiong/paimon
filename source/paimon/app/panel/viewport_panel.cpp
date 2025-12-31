#include "paimon/app/panel/viewport_panel.h"

#include <imgui.h>

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"

namespace paimon {

ViewportPanel::ViewportPanel() = default;

ViewportPanel::~ViewportPanel() = default;

void ViewportPanel::onImGuiRender() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Viewport");
  
  // Track focus state
  m_isFocused = ImGui::IsWindowFocused();
  
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
  
  // Viewport content will be rendered by Renderer in its onImGuiRender
  
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace paimon
