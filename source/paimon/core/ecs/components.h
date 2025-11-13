#pragma once

#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace paimon {
namespace ecs {

// ID component
struct IDComponent {
  uint64_t id = 0;
};

// Tag component
struct TagComponent {
  std::string tag;
};

// Transform component
struct TransformComponent {
  glm::vec3 position = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // identity quaternion
  glm::vec3 scale = glm::vec3(1.0f);

  glm::mat4 getTransformMatrix() const;
};

// Hierarchy component
struct HierarchyComponent {
  entt::entity parent = entt::null;
  std::vector<entt::entity> children;
};

// Camera component
struct CameraComponent {
  float fov = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 1000.0f;
  bool isPrimary = true;

  glm::mat4 getProjectionMatrix(float aspectRatio) const;
};

// Mesh component
struct MeshComponent {
  uint32_t meshHandle = 0;
  uint32_t materialHandle = 0;
};

// Light component
struct LightComponent {
  enum class Type {
    Directional,
    Point,
    Spot
  } type = Type::Directional;

  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 1.0f;
  
  // For point and spot lights
  float range = 10.0f;
  
  // For spot lights
  float innerConeAngle = 15.0f;
  float outerConeAngle = 30.0f;
};

} // namespace ecs
} // namespace paimon
