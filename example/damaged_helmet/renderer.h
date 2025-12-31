#pragma once

#include "paimon/app/layer.h"
#include "paimon/rendering/render_context.h"

#include "color_pass.h"
#include "final_pass.h"

namespace paimon {

class Renderer : public Layer {
public:
  Renderer();

  void onAttach() override;
  void onDetach() override {}
  void onUpdate() override;
  void onEvent(Event &event) override;
  void onImGuiRender() override;
private:
  std::unique_ptr<RenderContext> m_renderContext;
  
  glm::ivec2 m_resolution;

  ColorPass m_color_pass;
  FinalPass m_final_pass;

};
}