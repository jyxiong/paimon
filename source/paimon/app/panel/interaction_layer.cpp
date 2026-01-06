#include "paimon/app/panel/interaction_layer.h"

namespace paimon {

InteractionLayer::InteractionLayer() 
    : Layer("InteractionLayer"),
      m_cameraController(std::make_unique<OrbitCameraController>()) {}

InteractionLayer::~InteractionLayer() = default;

void InteractionLayer::onAttach() {}

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
