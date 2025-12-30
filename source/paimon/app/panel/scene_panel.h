#pragma once

#include "paimon/core/ecs/entity.h"

namespace paimon {

class ScenePanel {
public:
  ScenePanel();
  ~ScenePanel();

  void onImGuiRender();

private:
  void drawEntityNode(ecs::Entity entity);
  void drawEntityTransform(ecs::Entity entity);
  void drawComponents(ecs::Entity entity);
  void drawAddComponentButton(ecs::Entity entity);

  ecs::Entity m_selectedEntity;
};
}; // namespace paimon