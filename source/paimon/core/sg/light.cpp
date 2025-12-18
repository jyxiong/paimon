#include "paimon/core/sg/light.h"

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

float SpotLight::getAngleScale() const {
  return 1.0f / std::max(0.001f, std::cos(innerConeAngle) - std::cos(outerConeAngle));
}

float SpotLight::getAngleOffset() const {
  return -std::cos(outerConeAngle) * getAngleScale();
}

} // namespace sg
} // namespace paimon
