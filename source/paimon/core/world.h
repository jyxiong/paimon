#pragma once

#include <glm/glm.hpp>

namespace paimon {
class World {
public:
  // Direction vectors (right-handed coordinate system, Y-up)
  static constexpr glm::vec3 Origin = glm::vec3(0.0f, 0.0f, 0.0f);
  static constexpr glm::vec3 Forward =
      glm::vec3(0.0f, 0.0f, -1.0f); // -Z is forward
  static constexpr glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f); // +X is right
  static constexpr glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);    // +Y is up
};
} // namespace paimon
