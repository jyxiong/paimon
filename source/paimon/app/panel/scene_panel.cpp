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
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
                    cameraComp.position.x, cameraComp.position.y, cameraComp.position.z);
        ImGui::Text("Direction: (%.2f, %.2f, %.2f)", 
                    cameraComp.direction.x, cameraComp.direction.y, cameraComp.direction.z);
      } else {
        ImGui::Text("No camera object assigned");
      }
    }
  }
  
  // Mesh component
  if (entity.hasComponent<ecs::Mesh>()) {
    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &meshComp = entity.getComponent<ecs::Mesh>();
      
      if (meshComp.mesh) {
        ImGui::Text("Primitives: %zu", meshComp.mesh->primitives.size());
        
        ImGui::Separator();
        
        // Display each primitive
        for (size_t i = 0; i < meshComp.mesh->primitives.size(); ++i) {
          const auto &primitive = meshComp.mesh->primitives[i];
          
          if (ImGui::TreeNode((void*)(intptr_t)i, "Primitive %zu", i)) {
            // Topology mode
            const char* topologyName = "Unknown";
            switch (primitive.mode) {
              case PrimitiveTopology::Points: topologyName = "Points"; break;
              case PrimitiveTopology::Lines: topologyName = "Lines"; break;
              case PrimitiveTopology::LineLoop: topologyName = "Line Loop"; break;
              case PrimitiveTopology::LineStrip: topologyName = "Line Strip"; break;
              case PrimitiveTopology::Triangles: topologyName = "Triangles"; break;
              case PrimitiveTopology::TriangleStrip: topologyName = "Triangle Strip"; break;
              case PrimitiveTopology::TriangleFan: topologyName = "Triangle Fan"; break;
              default: break;
            }
            ImGui::Text("Topology: %s", topologyName);
            
            // Vertex and index counts
            ImGui::Text("Vertices: %zu", primitive.vertexCount);
            if (primitive.hasIndices()) {
              ImGui::Text("Indices: %zu", primitive.indexCount);
            } else {
              ImGui::Text("Indices: None");
            }
            
            ImGui::Separator();
            
            // Vertex attributes
            ImGui::Text("Vertex Attributes:");
            ImGui::Indent();
            ImGui::Text("Positions: %s", primitive.positions ? "Yes" : "No");
            ImGui::Text("Normals: %s", primitive.normals ? "Yes" : "No");
            ImGui::Text("Texcoords: %s", primitive.texcoords ? "Yes" : "No");
            ImGui::Text("Colors: %s", primitive.colors ? "Yes" : "No");
            ImGui::Unindent();
            
            ImGui::Separator();
            
            // Material
            if (primitive.material) {
              ImGui::Text("Material: Assigned");
            } else {
              ImGui::Text("Material: None");
            }
            
            ImGui::TreePop();
          }
        }
      } else {
        ImGui::Text("No mesh object assigned");
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
  
  // Light component
  if (entity.hasComponent<ecs::PunctualLight>()) {
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto &lightComp = entity.getComponent<ecs::PunctualLight>();
      
      if (lightComp.light) {
        auto lightType = lightComp.light->getType();
        
        // Light type dropdown
        const char* types[] = { "Directional", "Point", "Spot" };
        int currentType = static_cast<int>(lightType);
        
        if (ImGui::Combo("Type", &currentType, types, 3)) {
          // Switch light type
          switch (currentType) {
            case 0: // Directional
              if (lightType != sg::PunctualLight::Type::Directional) {
                lightComp.light = std::make_shared<sg::DirectionalLight>();
              }
              break;
            case 1: // Point
              if (lightType != sg::PunctualLight::Type::Point) {
                lightComp.light = std::make_shared<sg::PointLight>();
              }
              break;
            case 2: // Spot
              if (lightType != sg::PunctualLight::Type::Spot) {
                lightComp.light = std::make_shared<sg::SpotLight>();
              }
              break;
          }
        }
        
        ImGui::Separator();
        
        // Common light properties
        ImGui::ColorEdit3("Color", glm::value_ptr(lightComp.light->color));
        ImGui::DragFloat("Intensity", &lightComp.light->intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Range", &lightComp.light->range, 0.1f, 0.0f, 1000.0f);
        if (ImGui::IsItemHovered()) {
          ImGui::SetTooltip("0 = infinite range");
        }
        
        ImGui::Separator();
        
        // Type-specific properties
        if (lightType == sg::PunctualLight::Type::Directional) {
          ImGui::Text("Direction: (%.2f, %.2f, %.2f)",
                      lightComp.direction.x, lightComp.direction.y, lightComp.direction.z);
          ImGui::TextWrapped("Tip: Direction is controlled by Transform rotation");
        }
        else if (lightType == sg::PunctualLight::Type::Point) {
          ImGui::Text("Position: (%.2f, %.2f, %.2f)",
                      lightComp.position.x, lightComp.position.y, lightComp.position.z);
          ImGui::TextWrapped("Tip: Position is controlled by Transform translation");
        }
        else if (lightType == sg::PunctualLight::Type::Spot) {
          auto* spotLight = dynamic_cast<sg::SpotLight*>(lightComp.light.get());
          if (spotLight) {
            ImGui::Text("Position: (%.2f, %.2f, %.2f)",
                        lightComp.position.x, lightComp.position.y, lightComp.position.z);
            ImGui::Text("Direction: (%.2f, %.2f, %.2f)",
                        lightComp.direction.x, lightComp.direction.y, lightComp.direction.z);
            
            ImGui::Separator();
            
            // Convert cone angles to degrees for display
            float innerDegrees = glm::degrees(spotLight->innerConeAngle);
            float outerDegrees = glm::degrees(spotLight->outerConeAngle);
            
            if (ImGui::SliderFloat("Inner Cone Angle", &innerDegrees, 0.0f, 90.0f, "%.1f°")) {
              spotLight->innerConeAngle = glm::radians(innerDegrees);
              // Ensure inner <= outer
              if (spotLight->innerConeAngle > spotLight->outerConeAngle) {
                spotLight->outerConeAngle = spotLight->innerConeAngle;
              }
            }
            
            if (ImGui::SliderFloat("Outer Cone Angle", &outerDegrees, 0.0f, 90.0f, "%.1f°")) {
              spotLight->outerConeAngle = glm::radians(outerDegrees);
              // Ensure outer >= inner
              if (spotLight->outerConeAngle < spotLight->innerConeAngle) {
                spotLight->innerConeAngle = spotLight->outerConeAngle;
              }
            }
            
            ImGui::TextWrapped("Tip: Position and direction are controlled by Transform");
          }
        }
        
      } else {
        ImGui::Text("No light object assigned");
      }
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
