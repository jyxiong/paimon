#include "paimon/app/panel/viewport_panel.h"

#include <imgui.h>

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"
#include "paimon/app/event/mouse_event.h"
#include "paimon/app/mouse_code.h"

namespace paimon {

ViewportPanel::ViewportPanel() = default;

ViewportPanel::~ViewportPanel() = default;

void ViewportPanel::onImGuiRender() {
  auto &app = Application::getInstance();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Viewport");

  // Get available content region
  ImVec2 viewportSize = ImGui::GetContentRegionAvail();

  auto textureId =
      app.getRenderer().getColorPass().getColorTexture()->get_name();

  // Display the texture using ImGui::Image
  // Note: OpenGL uses bottom-left origin, so flip UV coordinates
  ImGui::Image((ImTextureID)(intptr_t)textureId, viewportSize,
                ImVec2(0, 1), // UV top-left (flipped)
                ImVec2(1, 0)  // UV bottom-right (flipped)
  );

  // Check if viewport size changed
  glm::ivec2 newSize = glm::ivec2(viewportSize.x, viewportSize.y);
  if (newSize.x > 0 && newSize.y > 0 && newSize != m_viewportSize) {
    m_viewportSize = newSize;

    // Dispatch viewport resize event
    ViewportResizeEvent event(newSize.x, newSize.y);
    app.onEvent(event);
  }

  // Handle camera control input when viewport is hovered
  if (ImGui::IsWindowHovered()) {
    auto &io = ImGui::GetIO();

    // Handle right mouse button (camera rotation)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
      m_isRightButtonPressed = true;
      MouseButtonPressedEvent event(MouseCode::Right);
      app.onEvent(event);
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
      m_isRightButtonPressed = false;
      MouseButtonReleasedEvent event(MouseCode::Right);
      app.onEvent(event);
    }

    // Handle middle mouse button (camera panning)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
      m_isMiddleButtonPressed = true;
      MouseButtonPressedEvent event(MouseCode::Middle);
      app.onEvent(event);
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) {
      m_isMiddleButtonPressed = false;
      MouseButtonReleasedEvent event(MouseCode::Middle);
      app.onEvent(event);
    }

    // Handle mouse movement (only when right or middle button is pressed)
    if (m_isRightButtonPressed || m_isMiddleButtonPressed) {
      ImVec2 mousePos = ImGui::GetMousePos();
      ImVec2 windowPos = ImGui::GetWindowPos();
      ImVec2 relativePos =
          ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

      MouseMovedEvent event(relativePos.x, relativePos.y);
      app.onEvent(event);
    }

    // Handle mouse scroll (camera zoom)
    if (io.MouseWheel != 0.0f) {
      MouseScrolledEvent event(0.0f, io.MouseWheel);
      app.onEvent(event);
    }
  }

  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace paimon
