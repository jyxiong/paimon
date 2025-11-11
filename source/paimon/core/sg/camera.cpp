#include "paimon/core/sg/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace paimon {
namespace sg {

glm::mat4 PerspectiveCamera::GetProjectionMatrix(float viewport_aspect) const {
  float aspect = (aspect_ratio > 0.0f) ? aspect_ratio : viewport_aspect;
  if (aspect <= 0.0f) {
    aspect = 16.0f / 9.0f; // Default fallback
  }

  if (zfar > 0.0f) {
    return glm::perspective(yfov, aspect, znear, zfar);
  } else {
    return glm::infinitePerspective(yfov, aspect, znear);
  }
}

glm::mat4 OrthographicCamera::GetProjectionMatrix(float viewport_aspect) const {
  // Orthographic camera ignores viewport_aspect parameter
  return glm::ortho(-xmag, xmag, -ymag, ymag, znear, zfar);
}

} // namespace sg
} // namespace paimon
