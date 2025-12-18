#include "paimon/core/sg/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace paimon {
namespace sg {

glm::mat4 PerspectiveCamera::getProjection() const {
  return glm::perspective(yfov, aspect, znear, zfar == 0.0f ? std::numeric_limits<float>::infinity() : zfar);
}

glm::mat4 OrthographicCamera::getProjection() const {
  return glm::ortho(-xmag, xmag, -ymag, ymag, znear, zfar);
}

} // namespace sg
} // namespace paimon
