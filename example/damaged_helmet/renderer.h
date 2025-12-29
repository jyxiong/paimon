#pragma once

#include "paimon/app/layer.h"
#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/rendering/render_context.h"

#include "color_pass.h"
#include "final_pass.h"

namespace paimon {

class Renderer : public Layer {
public:
  Renderer();

  void onAttach() override {}
  void onDetach() override {}
  void onUpdate() override;
  void onEvent(Event &event) override {}
  void onImGuiRender() override {}

  void onResize(int32_t width, int32_t height);

  ecs::Entity loadScene(const std::filesystem::path &scenePath);

private:
  std::unique_ptr<RenderContext> m_renderContext;

  std::unique_ptr<ecs::Scene> m_scene;

  glm::ivec2 m_resolution;

  ColorPass m_color_pass;
  FinalPass m_final_pass;

};
}