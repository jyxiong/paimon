#pragma once

#include "paimon/app/layer.h"
#include "paimon/app/event/application_event.h"
#include "paimon/rendering/render_context.h"

#include "color_pass.h"
#include "final_pass.h"

namespace paimon {

class ViewportPanel;

class Renderer : public Layer {
public:
  Renderer();

  void onAttach() override {}
  void onDetach() override {}
  void onUpdate() override;
  void onEvent(Event &event) override;
  void onImGuiRender() override {}

  void setViewportPanel(ViewportPanel* viewport) { m_viewportPanel = viewport; }

private:
  bool onViewportResize(const ViewportResizeEvent& event);
  
private:
  std::unique_ptr<RenderContext> m_renderContext;
  
  ViewportPanel* m_viewportPanel = nullptr;

  glm::ivec2 m_resolution;

  ColorPass m_color_pass;
  FinalPass m_final_pass;

};
}