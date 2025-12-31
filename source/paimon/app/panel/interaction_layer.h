#pragma once

#include "paimon/app/layer.h"
#include "paimon/core/camera/orbit_camera_controller.h"

namespace paimon {

class InteractionLayer : public Layer {
public:
  InteractionLayer();
  ~InteractionLayer() override;

  void onAttach() override;
  void onDetach() override;
  void onUpdate() override;
  void onEvent(Event &event) override;
  void onImGuiRender() override;

private:
  // Camera control
  std::unique_ptr<OrbitCameraController> m_cameraController;
};

} // namespace paimon
