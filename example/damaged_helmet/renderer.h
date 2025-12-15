#pragma once

#include "paimon/rendering/render_context.h"
#include "paimon/core/fg/transient_resources.h"

namespace paimon {

class Renderer {
public:
  Renderer(RenderContext &renderContext);

  void render();

private:
  RenderContext &m_renderContext;
  TransientResources m_transientResources;

  
  float m_rotation = 0.0f;
};
}