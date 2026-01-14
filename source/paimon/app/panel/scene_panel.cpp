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

  auto& scene = Application::getInstance().getScene();
  
  // Iterate through all entities and find root entities (no parent)
  auto view = scene.view<ecs::Name, ecs::Parent>();
  
  for (auto [entity, name, parent] : view.each()) {
    // Only draw root entities (entities without valid parent)
    if (!parent.parent.isValid()) {
      drawEntityNode(ecs::Entity(&scene, entity));
    }
  }

  // Deselect if clicking on empty space
  if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
    m_selectedEntity = {};
  }

  // Right-click context menu for creating entities
  if (ImGui::BeginPopupContextWindow("SceneContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
    if (ImGui::MenuItem("Create Empty Entity")) {
      auto newEntity = scene.createEntity("New Entity");
      m_selectedEntity = newEntity;
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
      auto& scene = Application::getInstance().getScene();
      auto child = scene.createEntity("Child Entity");
      // TODO: Set parent-child relationship
      // child.setParent(entity);
    }
    if (ImGui::MenuItem("Delete Entity")) {
      auto& scene = Application::getInstance().getScene();
      scene.destroyEntity(entity);
      if (m_selectedEntity == entity) {
        m_selectedEntity = {};
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
      auto &cameraComp = entity.getComponent<ecs::Camera>();
      
      if (cameraComp.camera) {
        auto cameraType = cameraComp.camera->getType();
        
        // Camera type dropdown
        const char* types[] = { "Perspective", "Orthographic" };
        int currentType = (cameraType == sg::Camera::Type::Perspective) ? 0 : 1;
        
        if (ImGui::Combo("Type", &currentType, types, 2)) {
          // Switch camera type
          if (currentType == 0 && cameraType != sg::Camera::Type::Perspective) {
            cameraComp.camera = std::make_shared<sg::PerspectiveCamera>();
          } else if (currentType == 1 && cameraType != sg::Camera::Type::Orthographic) {
            cameraComp.camera = std::make_shared<sg::OrthographicCamera>();
          }
        }
        
        ImGui::Separator();
        
        // Perspective camera properties
        if (cameraComp.camera->getType() == sg::Camera::Type::Perspective) {
          auto* perspCamera = dynamic_cast<sg::PerspectiveCamera*>(cameraComp.camera.get());
          if (perspCamera) {
            // Convert FOV to degrees for display
            float fovDegrees = glm::degrees(perspCamera->yfov);
            if (ImGui::SliderFloat("Field of View", &fovDegrees, 1.0f, 120.0f, "%.1f°")) {
              perspCamera->yfov = glm::radians(fovDegrees);
            }
            
            ImGui::DragFloat("Aspect Ratio", &perspCamera->aspectRatio, 0.01f, 0.1f, 10.0f);
            if (ImGui::IsItemHovered()) {
              ImGui::SetTooltip("0 = use viewport aspect ratio");
            }
            
            ImGui::DragFloat("Near Plane", &perspCamera->znear, 0.01f, 0.001f, 10.0f, "%.3f");
            ImGui::DragFloat("Far Plane", &perspCamera->zfar, 1.0f, 1.0f, 1000.0f);
            if (ImGui::IsItemHovered()) {
              ImGui::SetTooltip("0 = infinite far plane");
            }
          }
        }
        // Orthographic camera properties
        else if (cameraComp.camera->getType() == sg::Camera::Type::Orthographic) {
          auto* orthoCamera = dynamic_cast<sg::OrthographicCamera*>(cameraComp.camera.get());
          if (orthoCamera) {
            ImGui::DragFloat("Horizontal Mag", &orthoCamera->xmag, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Vertical Mag", &orthoCamera->ymag, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Near Plane", &orthoCamera->znear, 0.01f, 0.001f, 10.0f, "%.3f");
            ImGui::DragFloat("Far Plane", &orthoCamera->zfar, 1.0f, 1.0f, 1000.0f);
          }
        }
        
        ImGui::Separator();
        
        // Display camera vectors (read-only)
        // ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
        //             cameraComp.position.x, cameraComp.position.y, cameraComp.position.z);
        // ImGui::Text("Direction: (%.2f, %.2f, %.2f)", 
        //             cameraComp.direction.x, cameraComp.direction.y, cameraComp.direction.z);
      } else {
        ImGui::Text("No camera object assigned");
      }
    }
  }
  
  // Primitive component
  if (entity.hasComponent<ecs::Primitive>()) {
    if (ImGui::CollapsingHeader("Primitive", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &primitiveComp = entity.getComponent<ecs::Primitive>();
      
      if (primitiveComp.primitive) {
        auto &primitive = *primitiveComp.primitive;
        
        // Topology
        const char* topologyNames[] = { 
          "Points", "Lines", "Line Loop", "Line Strip", 
          "Triangles", "Triangle Strip", "Triangle Fan" 
        };
        int topologyIndex = static_cast<int>(primitive.mode);
        ImGui::Text("Topology: %s", topologyNames[topologyIndex]);
        
        ImGui::Separator();
        
        // Vertex data
        ImGui::Text("Vertex Count: %zu", primitive.vertexCount);
        
        ImGui::Text("Attributes:");
        ImGui::Indent();
        ImGui::Text("Positions: %s", primitive.positions ? "✓" : "✗");
        ImGui::Text("Normals: %s", primitive.normals ? "✓" : "✗");
        ImGui::Text("Texcoords: %s", primitive.texcoords ? "✓" : "✗");
        ImGui::Text("Colors: %s", primitive.colors ? "✓" : "✗");
        ImGui::Unindent();
        
        ImGui::Separator();
        
        // Index data
        if (primitive.hasIndices()) {
          ImGui::Text("Index Count: %zu", primitive.indexCount);
          
          // Map DataType enum to display string
          const char* indexTypeName = "Unknown";
          switch (primitive.indexType) {
            case DataType::Byte: indexTypeName = "Byte"; break;
            case DataType::UByte: indexTypeName = "UByte"; break;
            case DataType::Short: indexTypeName = "Short"; break;
            case DataType::UShort: indexTypeName = "UShort"; break;
            case DataType::Int: indexTypeName = "Int"; break;
            case DataType::UInt: indexTypeName = "UInt"; break;
            case DataType::Float: indexTypeName = "Float"; break;
            case DataType::Double: indexTypeName = "Double"; break;
          }
          ImGui::Text("Index Type: %s", indexTypeName);
        } else {
          ImGui::Text("No Indices (Non-indexed rendering)");
        }
        
      } else {
        ImGui::Text("No primitive object assigned");
      }
    }
  }
  
  // Material component
  if (entity.hasComponent<ecs::Material>()) {
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &materialComp = entity.getComponent<ecs::Material>();
      
      if (materialComp.material) {
        auto &material = *materialComp.material;
        
        // PBR Metallic-Roughness
        if (ImGui::TreeNodeEx("PBR Metallic-Roughness", ImGuiTreeNodeFlags_DefaultOpen)) {
          auto &pbr = material.pbrMetallicRoughness;
          
          ImGui::ColorEdit4("Base Color", glm::value_ptr(pbr.baseColorFactor));
          ImGui::Text("Base Color Texture: %s", pbr.baseColorTexture ? "Assigned" : "None");
          
          ImGui::SliderFloat("Metallic", &pbr.metallicFactor, 0.0f, 1.0f);
          ImGui::SliderFloat("Roughness", &pbr.roughnessFactor, 0.0f, 1.0f);
          ImGui::Text("Metallic-Roughness Texture: %s", 
                      pbr.metallicRoughnessTexture ? "Assigned" : "None");
          
          ImGui::TreePop();
        }
        
        // Normal Mapping
        if (ImGui::TreeNodeEx("Normal Mapping", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("Normal Texture: %s", material.normalTexture ? "Assigned" : "None");
          if (material.normalTexture) {
            ImGui::SliderFloat("Normal Scale", &material.normalScale, 0.0f, 2.0f);
          }
          ImGui::TreePop();
        }
        
        // Occlusion Mapping
        if (ImGui::TreeNodeEx("Occlusion Mapping", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("Occlusion Texture: %s", material.occlusionTexture ? "Assigned" : "None");
          if (material.occlusionTexture) {
            ImGui::SliderFloat("Occlusion Strength", &material.occlusionStrength, 0.0f, 1.0f);
          }
          ImGui::TreePop();
        }
        
        // Emissive
        if (ImGui::TreeNodeEx("Emissive", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::ColorEdit3("Emissive Factor", glm::value_ptr(material.emissiveFactor));
          ImGui::Text("Emissive Texture: %s", material.emissiveTexture ? "Assigned" : "None");
          ImGui::TreePop();
        }
        
        ImGui::Separator();
        
        // Alpha Mode
        const char* alphaModes[] = { "Opaque", "Mask", "Blend" };
        int currentAlphaMode = static_cast<int>(material.alphaMode);
        if (ImGui::Combo("Alpha Mode", &currentAlphaMode, alphaModes, 3)) {
          material.alphaMode = static_cast<sg::AlphaMode>(currentAlphaMode);
        }
        
        if (material.alphaMode == sg::AlphaMode::Mask) {
          ImGui::SliderFloat("Alpha Cutoff", &material.alphaCutoff, 0.0f, 1.0f);
        }
        
        ImGui::Checkbox("Double Sided", &material.doubleSided);
        
        // Anisotropy (KHR_materials_anisotropy)
        if (ImGui::TreeNode("Anisotropy")) {
          ImGui::SliderFloat("Strength##Aniso", &material.anisotropy.strength, 0.0f, 1.0f);
          ImGui::SliderFloat("Rotation##Aniso", &material.anisotropy.rotation, 0.0f, 360.0f, "%.1f°");
          ImGui::Text("Texture: %s", material.anisotropy.texture ? "Assigned" : "None");
          ImGui::TreePop();
        }
        
        // Clearcoat (KHR_materials_clearcoat)
        if (ImGui::TreeNode("Clearcoat")) {
          ImGui::SliderFloat("Factor##Clearcoat", &material.clearcoat.factor, 0.0f, 1.0f);
          ImGui::Text("Texture: %s", material.clearcoat.texture ? "Assigned" : "None");
          ImGui::SliderFloat("Roughness##Clearcoat", &material.clearcoat.roughnessFactor, 0.0f, 1.0f);
          ImGui::Text("Roughness Texture: %s", 
                      material.clearcoat.roughnessTexture ? "Assigned" : "None");
          ImGui::Text("Normal Texture: %s", 
                      material.clearcoat.normalTexture ? "Assigned" : "None");
          if (material.clearcoat.normalTexture) {
            ImGui::SliderFloat("Normal Scale##Clearcoat", &material.clearcoat.normalScale, 0.0f, 2.0f);
          }
          ImGui::TreePop();
        }
        
      } else {
        ImGui::Text("No material object assigned");
      }
    }
  }
  
  // Directional Light component
  if (entity.hasComponent<ecs::DirectionalLight>()) {
    if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &light = entity.getComponent<ecs::DirectionalLight>();
      
      ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
      ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.0f, 100.0f);
      ImGui::TextWrapped("Tip: Direction is controlled by Transform rotation");
    }
  }
  
  // Point Light component
  if (entity.hasComponent<ecs::PointLight>()) {
    if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &light = entity.getComponent<ecs::PointLight>();
      
      ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
      ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.0f, 100.0f);
      ImGui::DragFloat("Range", &light.range, 0.1f, 0.0f, 1000.0f);
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("0 = infinite range");
      }
      ImGui::TextWrapped("Tip: Position is controlled by Transform translation");
    }
  }
  
  // Spot Light component
  if (entity.hasComponent<ecs::SpotLight>()) {
    if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &light = entity.getComponent<ecs::SpotLight>();
      
      ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
      ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.0f, 100.0f);
      ImGui::DragFloat("Range", &light.range, 0.1f, 0.0f, 1000.0f);
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("0 = infinite range");
      }
      
      ImGui::Separator();
      
      float innerDegrees = glm::degrees(light.innerConeAngle);
      float outerDegrees = glm::degrees(light.outerConeAngle);
      
      if (ImGui::SliderFloat("Inner Cone Angle", &innerDegrees, 0.0f, 90.0f, "%.1f°")) {
        light.innerConeAngle = glm::radians(innerDegrees);
        // Ensure inner <= outer
        if (light.innerConeAngle > light.outerConeAngle) {
          light.outerConeAngle = light.innerConeAngle;
        }
      }
      
      if (ImGui::SliderFloat("Outer Cone Angle", &outerDegrees, 0.0f, 90.0f, "%.1f°")) {
        light.outerConeAngle = glm::radians(outerDegrees);
        // Ensure outer >= inner
        if (light.outerConeAngle < light.innerConeAngle) {
          light.innerConeAngle = light.outerConeAngle;
        }
      }
      
      ImGui::TextWrapped("Tip: Position and direction are controlled by Transform");
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
    
    if (!entity.hasComponent<ecs::Primitive>()) {
      if (ImGui::MenuItem("Primitive")) {
        entity.addComponent<ecs::Primitive>();
      }
    }
    
    if (!entity.hasComponent<ecs::Material>()) {
      if (ImGui::MenuItem("Material")) {
        entity.addComponent<ecs::Material>();
      }
    }
    
    // Light submenu
    if (ImGui::BeginMenu("Light")) {
      if (!entity.hasComponent<ecs::DirectionalLight>()) {
        if (ImGui::MenuItem("Directional Light")) {
          entity.addComponent<ecs::DirectionalLight>();
        }
      }
      if (!entity.hasComponent<ecs::PointLight>()) {
        if (ImGui::MenuItem("Point Light")) {
          entity.addComponent<ecs::PointLight>();
        }
      }
      if (!entity.hasComponent<ecs::SpotLight>()) {
        if (ImGui::MenuItem("Spot Light")) {
          entity.addComponent<ecs::SpotLight>();
        }
      }
      ImGui::EndMenu();
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
