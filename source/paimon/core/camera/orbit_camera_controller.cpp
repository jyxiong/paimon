#include "paimon/core/camera/orbit_camera_controller.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "paimon/app/event/application_event.h"
#include "paimon/app/event/event.h"
#include "paimon/app/event/mouse_event.h"
#include "paimon/app/mouse_code.h"
#include "paimon/core/ecs/components.h"
#include "paimon/core/sg/camera.h"

namespace paimon {

OrbitCameraController::OrbitCameraController()
  : m_targetPosition(0.0f), m_distance(5.0f), m_yaw(0.0f), m_pitch(0.0f) {}

void OrbitCameraController::onEvent(Event& event) {
  if (!m_enabled || !m_camera.isValid()) {
    return;
  }

  EventDispatcher dispatcher(event);
  
  // Handle viewport resize
  dispatcher.dispatch<ViewportResizeEvent>([this](const ViewportResizeEvent& e) {
    onResize(glm::ivec2(e.getWidth(), e.getHeight()));
    return false; // Allow other handlers to process this event
  });
  
  // Handle mouse button press
  dispatcher.dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& e) {
    if (e.getMouseButton() == MouseCode::Right) {
      m_isRotating = true;
      m_lastMousePos = glm::vec2(0.0f); // Will be initialized on first move
      return true;
    }
    if (e.getMouseButton() == MouseCode::Middle) {
      m_isPanning = true;
      m_lastMousePos = glm::vec2(0.0f); // Will be initialized on first move
      return true;
    }
    return false;
  });
  
  // Handle mouse button release
  dispatcher.dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent& e) {
    if (e.getMouseButton() == MouseCode::Right) {
      m_isRotating = false;
      return true;
    }
    if (e.getMouseButton() == MouseCode::Middle) {
      m_isPanning = false;
      return true;
    }
    return false;
  });
  
  // Handle mouse movement
  dispatcher.dispatch<MouseMovedEvent>([this](const MouseMovedEvent& e) {
    glm::vec2 mousePos(e.getX(), e.getY());
    glm::vec2 mouseDelta = mousePos - m_lastMousePos;
    
    // Initialize on first move after button press
    if (m_lastMousePos == glm::vec2(0.0f)) {
      m_lastMousePos = mousePos;
      return false;
    }
    
    if (m_isRotating && glm::length(mouseDelta) > 0.01f) {
      onRotate(mouseDelta);
      m_lastMousePos = mousePos;
      return true;
    }
    
    if (m_isPanning && glm::length(mouseDelta) > 0.01f) {
      onPan(mouseDelta);
      m_lastMousePos = mousePos;
      return true;
    }
    
    m_lastMousePos = mousePos;
    return false;
  });
  
  // Handle mouse scroll
  dispatcher.dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& e) {
    onZoom(e.getYOffset());
    return true;
  });
}

void OrbitCameraController::onRotate(const glm::vec2& delta) {
  // Right-click drag - rotate view
  m_yaw -= delta.x * m_sensitivity;
  m_pitch -= delta.y * m_sensitivity;
  
  // Clamp pitch angle
  m_pitch = glm::clamp(m_pitch, glm::radians(m_minPitch), glm::radians(m_maxPitch));
}

void OrbitCameraController::onPan(const glm::vec2& delta) {
  // Middle-click drag - pan view
  m_targetPosition += calculatePanDelta(delta);
}

void OrbitCameraController::onZoom(float scrollDelta) {
  // Mouse wheel - zoom
  m_distance *= (1.0f - scrollDelta * m_zoomSpeed);
  
  // Clamp distance
  m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
}

void OrbitCameraController::update(float deltaTime) {
  if (!m_enabled || !m_camera.isValid()) {
    return;
  }

  updateCameraTransform();
}

void OrbitCameraController::updateCameraTransform() {
  if (!m_camera.hasComponent<ecs::Transform>()) {
    return;
  }

  // Calculate camera position in spherical coordinates
  float cosYaw = glm::cos(m_yaw);
  float sinYaw = glm::sin(m_yaw);
  float cosPitch = glm::cos(m_pitch);
  float sinPitch = glm::sin(m_pitch);

  glm::vec3 offset(
    m_distance * cosPitch * cosYaw,
    m_distance * sinPitch,
    m_distance * cosPitch * sinYaw
  );

  glm::vec3 cameraPosition = m_targetPosition + offset;
  glm::vec3 direction = glm::normalize(m_targetPosition - cameraPosition);

  // Calculate camera rotation (looking at target)
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 right = glm::normalize(glm::cross(direction, up));
  glm::vec3 cameraUp = glm::cross(right, direction);

  glm::mat3 rotationMatrix(right, cameraUp, -direction);
  glm::quat rotation = glm::quat_cast(rotationMatrix);

  // Update Transform component
  auto& transform = m_camera.getComponent<ecs::Transform>();
  transform.translation = cameraPosition;
  transform.rotation = rotation;

  // Update Camera component
  if (m_camera.hasComponent<ecs::Camera>()) {
    auto& camera = m_camera.getComponent<ecs::Camera>();
    camera.position = cameraPosition;
    camera.direction = direction;
    
    // Update view matrix
    camera.view = glm::lookAt(cameraPosition, m_targetPosition, cameraUp);
    camera.inverseView = glm::inverse(camera.view);
  }
}

glm::vec3 OrbitCameraController::calculatePanDelta(const glm::vec2& delta) {
  if (!m_camera.hasComponent<ecs::Transform>()) {
    return glm::vec3(0.0f);
  }

  auto& transform = m_camera.getComponent<ecs::Transform>();
  
  // Get camera's right and up directions
  glm::vec3 forward = glm::normalize(m_targetPosition - transform.translation);
  glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
  glm::vec3 up = glm::cross(right, forward);

  // Adjust pan speed based on distance (move faster when farther away)
  float speedFactor = m_distance * m_panSpeed;

  // Calculate pan vector
  glm::vec3 panDelta = -right * delta.x * speedFactor + up * delta.y * speedFactor;
  
  return panDelta;
}

void OrbitCameraController::onResize(const glm::ivec2& resolution) {
  if (!m_camera.isValid() || !m_camera.hasComponent<ecs::Camera>()) {
    return;
  }
  
  auto& cameraComp = m_camera.getComponent<ecs::Camera>();
  if (!cameraComp.camera) return;
  
  // Calculate aspect ratio
  float aspectRatio = resolution.x > 0 && resolution.y > 0
    ? static_cast<float>(resolution.x) / static_cast<float>(resolution.y)
    : 1.0f;
  
  // Update aspect ratio for perspective cameras
  if (cameraComp.camera->getType() == sg::Camera::Type::Perspective) {
    auto* perspCam = static_cast<sg::PerspectiveCamera*>(cameraComp.camera.get());
    perspCam->aspectRatio = aspectRatio;
    cameraComp.projection = perspCam->getProjection();
  }
}

} // namespace paimon
