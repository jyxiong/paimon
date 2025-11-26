#pragma once

#include <memory>

#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/graphics_pipeline.h"

using namespace paimon;

class ScreenQuad {
public:
  ScreenQuad();

  void draw(const Texture &texture);

private:
  std::unique_ptr<VertexArray> m_vao;
  std::unique_ptr<Sampler> m_sampler;
  std::unique_ptr<GraphicsPipeline> m_pipeline;
};
