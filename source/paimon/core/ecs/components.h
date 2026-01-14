#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/sg/camera.h"
#include "paimon/core/sg/light.h"
#include "paimon/core/sg/material.h"
#include "paimon/core/sg/mesh.h"

namespace paimon {

namespace ecs {

class Entity;

/// Tag component for the entity name
struct Name {
  std::string name;
};

/// Parent component - stores the parent entity reference
struct Parent {
  Entity parent;
};

/// Children component - stores child entity references
struct Children {
  std::vector<Entity> children;
};

/// Transform component (TRS representation)
struct Transform {
  glm::vec3 translation = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // w, x, y, z
  glm::vec3 scale = glm::vec3(1.0f);
  
  glm::mat4 matrix() const {
    return glm::translate(glm::mat4(1.0f), translation) *
           glm::mat4_cast(rotation) *
           glm::scale(glm::mat4(1.0f), scale);
  }
};

struct GlobalTransform {
  glm::mat4 matrix = glm::mat4(1.0f);
};

/// Primitive component - references primitive data
struct Primitive {
  std::shared_ptr<sg::Primitive> primitive;
};

/// Material component - references material data
struct Material {
  std::shared_ptr<sg::Material> material;
};

/// Perspective camera parameters
struct Camera {
  std::shared_ptr<sg::Camera> camera;

  glm::mat4 view = glm::mat4{ 1.0f };
  glm::mat4 inverseView = glm::mat4{ 1.0f };
  glm::mat4 projection = glm::mat4{ 1.0f };
};

/// Directional light component
struct DirectionalLight {
  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 1.0f;
};

/// Point light component
struct PointLight {
  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 1.0f;
  float range = 0.0f; // 0.0 = infinite
};

/// Spot light component
struct SpotLight {
  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 1.0f;
  float range = 0.0f; // 0.0 = infinite
  float innerConeAngle = 0.0f;
  float outerConeAngle = glm::radians(45.0f);
};

/// Renderable component - marks entities that should be rendered
struct Renderable {
  bool visible = true;
  bool castShadow = true;
  bool receiveShadow = true;
};

} // namespace ecs
} // namespace paimon
