#include "paimon/core/sg/light.h"

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

float SpotLight::GetAngularAttenuation(float angle_from_direction) const {
  if (angle_from_direction < inner_cone_angle) {
    return 1.0f;
  } else if (angle_from_direction < outer_cone_angle) {
    // Smooth interpolation between inner and outer cone
    float t = (angle_from_direction - inner_cone_angle) /
              (outer_cone_angle - inner_cone_angle);
    t = glm::clamp(t, 0.0f, 1.0f);
    return 1.0f - t * t; // Quadratic falloff
  } else {
    return 0.0f;
  }
}

} // namespace sg
} // namespace paimon
