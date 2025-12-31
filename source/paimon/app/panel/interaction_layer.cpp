#include "paimon/app/panel/interaction_layer.h"

#include "paimon/app/application.h"

namespace paimon {

InteractionLayer::InteractionLayer() 
    : Layer("InteractionLayer"),
      m_cameraController(std::make_unique<OrbitCameraController>()) {}

InteractionLayer::~InteractionLayer() = default;

void InteractionLayer::onAttach() {
  // Bind camera to controller
  auto& scene = Application::getInstance().getScene();
  auto mainCamera = scene.getMainCamera();
  m_cameraController->CameraController::setTarget(mainCamera);
}

void InteractionLayer::onDetach() {}

void InteractionLayer::onUpdate() {
  // Update camera controller
  m_cameraController->update(0.016f); // ~60 FPS
}

void InteractionLayer::onEvent(Event &event) {
  // Forward input events to camera controller
  m_cameraController->onEvent(event);
}

void InteractionLayer::onImGuiRender() {
  // No GUI rendering needed for interaction layer
}

} // namespace paimon
