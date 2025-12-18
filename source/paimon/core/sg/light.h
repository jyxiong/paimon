#pragma once

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

/// Base punctual light struct (KHR_lights_punctual)
/// All lights emit light in a cone (or hemisphere for point lights)
struct PunctualLight {
  /// Light types based on KHR_lights_punctual
  enum struct Type {
    Directional, // Directional light (sun-like)
    Point,       // Point light (omni-directional)
    Spot         // Spot light (cone-shaped)
  };

  virtual ~PunctualLight() = default;

  // Light properties
  glm::vec3 color = glm::vec3(1.0f); // RGB color
  float intensity = 1.0f;            // Brightness multiplier
  float range = 0.0f;                // 0.0 = infinite range

  // Get light type
  virtual Type getType() const = 0;
};

/// Directional light (infinite distance, parallel rays)
struct DirectionalLight : public PunctualLight {

  ~DirectionalLight() = default;

  Type getType() const override { return Type::Directional; }
};

/// Point light (omni-directional, radiates in all directions)
struct PointLight : public PunctualLight {
  ~PointLight() = default;

  Type getType() const override { return Type::Point; }
};

/// Spot light (cone-shaped, directional with falloff)
struct SpotLight : public PunctualLight {
  float innerConeAngle = 0.0f;
  float outerConeAngle = glm::radians(45.0f);

  ~SpotLight() = default;

  Type getType() const override { return Type::Spot; }

  float getAngleScale() const;

  float getAngleOffset() const;
};

} // namespace sg
} // namespace paimon
