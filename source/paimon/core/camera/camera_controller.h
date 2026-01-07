#pragma once

#include "paimon/core/ecs/entity.h"
#include "paimon/app/event/event.h"

namespace paimon {

class CameraController {
public:
  CameraController() = default;
  virtual ~CameraController() = default;

  virtual void onEvent(Event& event) = 0;
  virtual void update(float deltaTime) = 0;

  void setEnabled(bool enabled) { m_enabled = enabled; }
  bool isEnabled() const { return m_enabled; }

protected:
  ecs::Entity m_camera;
  bool m_enabled = true;
};

} // namespace paimon
