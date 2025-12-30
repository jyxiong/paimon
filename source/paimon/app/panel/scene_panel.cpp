#include "paimon/app/panel/scene_panel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "paimon/app/application.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/ecs/components.h"

namespace paimon {

ScenePanel::ScenePanel() {}

ScenePanel::~ScenePanel() {}

void ScenePanel::onImGuiRender() {
  ImGui::Begin("Scene Hierarchy");

  auto* scene = Application::getInstance().getScene();
  
  if (scene) {
    // Iterate through all entities and find root entities (no parent)
    auto view = scene->view<ecs::Name, ecs::Parent>();
    
    for (auto [entity, name, parent] : view.each()) {
      // Only draw root entities (entities without valid parent)
      if (!parent.parent.isValid()) {
        drawEntityNode(ecs::Entity(scene, entity));
      }
    }
  }

  // Deselect if clicking on empty space
  if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
    m_selectedEntity = {};
  }

  // Right-click context menu for creating entities
  if (ImGui::BeginPopupContextWindow("SceneContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
    if (ImGui::MenuItem("Create Empty Entity")) {
      if (scene) {
        auto newEntity = scene->createEntity("New Entity");
        m_selectedEntity = newEntity;
      }
    }
    ImGui::EndPopup();
  }

  ImGui::End();

  // Property panel
  ImGui::Begin("Properties");
  
  if (m_selectedEntity.isValid()) {
    // Display entity name (editable)
    auto &name = m_selectedEntity.getComponent<ecs::Name>();
    char buffer[256];
    strncpy(buffer, name.name.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
      name.name = buffer;
    }
    
    ImGui::Separator();
    
    // Draw all components
    drawComponents(m_selectedEntity);
    
    // Add component button
    drawAddComponentButton(m_selectedEntity);
  }
  
  ImGui::End();
}

void ScenePanel::drawEntityNode(ecs::Entity entity) {
  if (!entity.isValid()) {
    return;
  }

  auto &name = entity.getComponent<ecs::Name>();
  auto *childrenComp = entity.tryGetComponent<ecs::Children>();

  // ImGui tree node flags
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | 
                              ImGuiTreeNodeFlags_OpenOnDoubleClick |
                              ImGuiTreeNodeFlags_SpanAvailWidth;

  // If this entity is selected, add the Selected flag
  if (m_selectedEntity == entity) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  // If no children, make it a leaf
  bool hasChildren = childrenComp && !childrenComp->children.empty();
  if (!hasChildren) {
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  // Draw tree node
  bool opened = ImGui::TreeNodeEx(
    (void*)(uint64_t)(uint32_t)entity.getHandle(), 
    flags, 
    "%s", 
    name.name.c_str()
  );

  // Handle selection
  if (ImGui::IsItemClicked()) {
    m_selectedEntity = entity;
  }

  // Right-click context menu on entity
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Create Child Entity")) {
      auto* scene = Application::getInstance().getScene();
      if (scene) {
        auto child = scene->createEntity("Child Entity");
        // TODO: Set parent-child relationship
        // child.setParent(entity);
      }
    }
    if (ImGui::MenuItem("Delete Entity")) {
      auto* scene = Application::getInstance().getScene();
      if (scene) {
        scene->destroyEntity(entity);
        if (m_selectedEntity == entity) {
          m_selectedEntity = {};
        }
      }
    }
    ImGui::EndPopup();
  }

  // Draw children if tree is opened
  if (opened && hasChildren) {
    for (auto &child : childrenComp->children) {
      drawEntityNode(child);
    }
    ImGui::TreePop();
  }
}

void ScenePanel::drawComponents(ecs::Entity entity) {
  // Transform component
  if (entity.hasComponent<ecs::Transform>()) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &transform = entity.getComponent<ecs::Transform>();
      
      ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.1f);
      
      // Rotation (as Euler angles in degrees)
      glm::vec3 rotation = glm::degrees(glm::eulerAngles(transform.rotation));
      if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f)) {
        transform.rotation = glm::quat(glm::radians(rotation));
      }
      
      ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);
    }
  }
  
  // Camera component
  if (entity.hasComponent<ecs::Camera>()) {
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Camera Component");
      // TODO: Add camera properties editing
    }
  }
  
  // Mesh component
  if (entity.hasComponent<ecs::Mesh>()) {
    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Mesh Component");
      // TODO: Add mesh properties
    }
  }
  
  // Material component
  if (entity.hasComponent<ecs::Material>()) {
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Material Component");
      // TODO: Add material properties editing
    }
  }
  
  // Light component
  if (entity.hasComponent<ecs::PunctualLight>()) {
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Light Component");
      // TODO: Add light properties editing
    }
  }
}

void ScenePanel::drawAddComponentButton(ecs::Entity entity) {
  ImGui::Separator();
  
  if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
    ImGui::OpenPopup("AddComponent");
  }
  
  if (ImGui::BeginPopup("AddComponent")) {
    if (!entity.hasComponent<ecs::Transform>()) {
      if (ImGui::MenuItem("Transform")) {
        entity.addComponent<ecs::Transform>();
      }
    }
    
    if (!entity.hasComponent<ecs::Camera>()) {
      if (ImGui::MenuItem("Camera")) {
        entity.addComponent<ecs::Camera>();
      }
    }
    
    if (!entity.hasComponent<ecs::Mesh>()) {
      if (ImGui::MenuItem("Mesh")) {
        entity.addComponent<ecs::Mesh>();
      }
    }
    
    if (!entity.hasComponent<ecs::Material>()) {
      if (ImGui::MenuItem("Material")) {
        entity.addComponent<ecs::Material>();
      }
    }
    
    if (!entity.hasComponent<ecs::PunctualLight>()) {
      if (ImGui::MenuItem("Light")) {
        entity.addComponent<ecs::PunctualLight>();
      }
    }
    
    ImGui::EndPopup();
  }
}

void ScenePanel::drawEntityTransform(ecs::Entity entity) {
  if (!entity.isValid()) {
    return;
  }

  // Display entity name
  auto &name = entity.getComponent<ecs::Name>();
  ImGui::Text("Entity: %s", name.name.c_str());
  ImGui::Separator();

  // Display and edit transform
  if (entity.hasComponent<ecs::Transform>()) {
    auto &transform = entity.getComponent<ecs::Transform>();

    ImGui::Text("Transform");
    
    // Translation
    ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.1f);
    
    // Rotation (as Euler angles in degrees)
    glm::vec3 rotation = glm::degrees(glm::eulerAngles(transform.rotation));
    if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f)) {
      transform.rotation = glm::quat(glm::radians(rotation));
    }
    
    // Scale
    ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);
  }
}

} // namespace paimon
