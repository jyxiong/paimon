#pragma once

#include <memory>

#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

namespace paimon {

class FinalPass {
public:
  FinalPass(RenderContext &renderContext);

  void draw(RenderContext& ctx, const Texture &texture, const glm::ivec2 &size);

private:
  RenderContext& m_renderContext;

  std::unique_ptr<Sampler> m_sampler;
  std::unique_ptr<GraphicsPipeline> m_pipeline;
};

} // namespace paimon
