#pragma once

#include <glm/glm.hpp>

#include "paimon/core/camera/camera_controller.h"

namespace paimon {

class OrbitCameraController : public CameraController {
public:
  OrbitCameraController();
  ~OrbitCameraController() override = default;

  void onEvent(Event& event) override;
  void update(float deltaTime) override;

private:
  void updateCameraTransform();
  glm::vec3 calculatePanDelta(const glm::vec2& delta);
  void onRotate(const glm::vec2& delta);
  void onPan(const glm::vec2& delta);
  void onZoom(float scrollDelta);
  void onResize(const glm::ivec2& resolution);

private:
  // Camera parameters
  glm::vec3 m_focusPoint = glm::vec3(0.0f);
  float m_distance = 5.0f;
  float m_yaw = 0.0f;      // Horizontal angle
  float m_pitch = 0.0f;    // Vertical angle

  // Control parameters
  float m_sensitivity = 0.003f;
  float m_zoomSpeed = 0.1f;
  float m_panSpeed = 0.005f;

  // Constraints
  float m_minDistance = 0.1f;
  float m_maxDistance = 100.0f;
  float m_minPitch = -89.0f;
  float m_maxPitch = 89.0f;
  
  // Input state
  glm::vec2 m_lastMousePos = glm::vec2(0.0f);
  bool m_isRotating = false;
  bool m_isPanning = false;
};

} // namespace paimon
