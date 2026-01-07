#pragma once

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

struct Camera {
  enum class Type { Perspective, Orthographic };

  Camera() = default;

  virtual ~Camera() = default;

  virtual Type getType() const = 0;

  virtual glm::mat4 getProjection() const = 0;
};

/// Perspective camera
struct PerspectiveCamera : public Camera {
  float aspectRatio = 1.0f;        // width/height, 0 = use viewport aspect
  float yfov = glm::radians(45.0f); // Vertical field of view in radians
  float znear = 0.1f;
  float zfar = 100.0f; // 0 = infinite

  PerspectiveCamera() = default;

  ~PerspectiveCamera() = default;

  Type getType() const override { return Type::Perspective; }

  glm::mat4 getProjection() const override;
};

/// Orthographic camera
struct OrthographicCamera : public Camera {
  float xmag = 1.0f; // Horizontal magnification
  float ymag = 1.0f; // Vertical magnification
  float znear = 0.1f;
  float zfar = 100.0f;

  OrthographicCamera() = default;

  ~OrthographicCamera() = default;

  Type getType() const override { return Type::Orthographic; }

  glm::mat4 getProjection() const override;
};

} // namespace sg
} // namespace paimon
