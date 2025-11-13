#include "paimon/core/ecs/components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace paimon {
namespace ecs {

glm::mat4 TransformComponent::getTransformMatrix() const {
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, position);
  transform = transform * glm::mat4_cast(rotation);
  transform = glm::scale(transform, scale);
  return transform;
}

glm::mat4 CameraComponent::getProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

} // namespace ecs
} // namespace paimon
