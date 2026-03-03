#include "paimon/app/panel/editor_layer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

namespace paimon {

EditorLayer::EditorLayer() : Layer("EditorLayer") {}

EditorLayer::~EditorLayer() = default;

void EditorLayer::onAttach() {
  // Panels will get scene directly from Application singleton
}

void EditorLayer::onDetach() {}

void EditorLayer::onUpdate() {}

void EditorLayer::onEvent(Event &event) {}

void EditorLayer::onImGuiRender() {
  // Setup default docking layout on first frame
  if (m_firstTime) {
    setupDockingLayout();
    m_firstTime = false;
  }
  
  m_menuPanel.onImGuiRender();
  m_viewportPanel.onImGuiRender();
  m_scenePanel.onImGuiRender();
}

void EditorLayer::setupDockingLayout() {
  // Get the main dockspace ID
  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  
  // Clear any existing layout
  ImGui::DockBuilderRemoveNode(dockspace_id);
  ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
  
  // Split the dockspace into left and right
  ImGuiID dock_left, dock_right;
  ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, &dock_left, &dock_right);

  // Split right into center and properties
  ImGuiID dock_center, dock_bottom_right;
  ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Right, 0.25f, &dock_bottom_right, &dock_center);

  // Split left into top (scene hierarchy)
  ImGuiID dock_left_top, dock_left_bottom;
  ImGui::DockBuilderSplitNode(dock_left, ImGuiDir_Down, 0.45f, &dock_left_bottom, &dock_left_top);

  // Dock windows to specific locations
  ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_left_top);
  ImGui::DockBuilderDockWindow("Viewport",        dock_center);
  ImGui::DockBuilderDockWindow("Properties",      dock_bottom_right);
  
  // Finish building
  ImGui::DockBuilderFinish(dockspace_id);
}

} // namespace paimon
