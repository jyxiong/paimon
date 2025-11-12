#pragma once

#include <string>

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

/// Base punctual light class (KHR_lights_punctual)
/// All lights emit light in a cone (or hemisphere for point lights)
class PunctualLight {
public:
  /// Light types based on KHR_lights_punctual
  enum class Type {
    Directional, // Directional light (sun-like)
    Point,       // Point light (omni-directional)
    Spot         // Spot light (cone-shaped)
  };

  std::string name;

  PunctualLight() = default;
  PunctualLight(const std::string &name) : name(name) {}
  virtual ~PunctualLight() = default;

  // Light properties
  glm::vec3 color = glm::vec3(1.0f); // RGB color
  float intensity = 1.0f;            // Brightness multiplier
  float range = 0.0f;                // 0.0 = infinite range

  // Get light type
  virtual Type GetType() const = 0;
};

/// Directional light (infinite distance, parallel rays)
class DirectionalLight : public PunctualLight {
public:
  DirectionalLight() = default;
  DirectionalLight(const std::string &name) : PunctualLight(name) {}

  Type GetType() const override { return Type::Directional; }

  // Direction is defined by the node's transform (forward = -Z axis)
};

/// Point light (omni-directional, radiates in all directions)
class PointLight : public PunctualLight {
public:
  PointLight() = default;
  PointLight(const std::string &name) : PunctualLight(name) {}

  Type GetType() const override { return Type::Point; }

  // Position is defined by the node's transform
};

/// Spot light (cone-shaped, directional with falloff)
class SpotLight : public PunctualLight {
public:
  float inner_cone_angle = 0.0f; // Inner cone angle in radians
  float outer_cone_angle =
    glm::radians(45.0f); // Outer cone angle in radians (default π/4)

  SpotLight() = default;
  SpotLight(const std::string &name) : PunctualLight(name) {}

  Type GetType() const override { return Type::Spot; }

  // Direction is defined by the node's transform (forward = -Z axis)
  // Position is defined by the node's transform

  /// Get the angular attenuation factor for a given angle from the light
  /// direction
  /// @param angle_from_direction Angle in radians from the light's forward
  /// direction
  /// @return Attenuation factor [0, 1]
  float GetAngularAttenuation(float angle_from_direction) const;
};

} // namespace sg
} // namespace paimon
