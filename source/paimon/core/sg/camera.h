#pragma once

#include <string>

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

/// Base camera class
struct Camera {
  /// Camera types
  enum class Type { Perspective, Orthographic };

  std::string name;

  Camera() = default;
  Camera(const std::string &name) : name(name) {}
  virtual ~Camera() = default;

  // Get camera type
  virtual Type GetType() const = 0;

  // Get projection matrix
  virtual glm::mat4 GetProjectionMatrix(float viewport_aspect = 0.0f) const = 0;
};

/// Perspective camera
struct PerspectiveCamera : public Camera {
  float aspect_ratio = 0.0f;        // width/height, 0 = use viewport aspect
  float yfov = glm::radians(45.0f); // Vertical field of view in radians
  float znear = 0.1f;
  float zfar = 0.0f; // 0 = infinite

  PerspectiveCamera() = default;
  PerspectiveCamera(const std::string &name) : Camera(name) {}

  Type GetType() const override { return Type::Perspective; }
  glm::mat4 GetProjectionMatrix(float viewport_aspect = 0.0f) const override;
};

/// Orthographic camera
struct OrthographicCamera : public Camera {
  float xmag = 1.0f; // Horizontal magnification
  float ymag = 1.0f; // Vertical magnification
  float znear = 0.1f;
  float zfar = 100.0f;

  OrthographicCamera() = default;
  OrthographicCamera(const std::string &name) : Camera(name) {}

  Type GetType() const override { return Type::Orthographic; }
  glm::mat4 GetProjectionMatrix(float viewport_aspect = 0.0f) const override;
};

} // namespace sg
} // namespace paimon
