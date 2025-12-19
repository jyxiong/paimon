#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "paimon/core/ecs/entity.h"
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

/// Hierarchy component - stores parent-child relationships
struct Hierarchy {
  Entity parent;
  std::vector<Entity> children;
};

/// Transform component (TRS representation)
struct Transform {
  glm::vec3 translation = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // w, x, y, z
  glm::vec3 scale = glm::vec3(1.0f);

  glm::mat4 matrix = glm::mat4{ 1.0f };
};

struct GlobalTransform {
  glm::mat4 matrix = glm::mat4{ 1.0f };
};

/// Mesh component - references mesh data
struct Mesh {
  std::shared_ptr<sg::Mesh> mesh;
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
struct PunctualLight {
  std::shared_ptr<sg::PunctualLight> light;
};

/// Renderable component - marks entities that should be rendered
struct Renderable {
  bool visible = true;
  bool castShadow = true;
  bool receiveShadow = true;
};

} // namespace ecs
} // namespace paimon
