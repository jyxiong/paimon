#pragma once

#include <memory>
#include "paimon/opengl/texture.h"

namespace paimon {

namespace sg {
struct Primitive;
}

class RenderContext;
class GraphicsPipeline;
class VertexArray;
class Framebuffer;
class Renderbuffer;
class Buffer;

/// Pass for generating BRDF integration lookup texture
class BRDFLUTPass {
public:
  BRDFLUTPass(RenderContext &renderContext);
  ~BRDFLUTPass();

  BRDFLUTPass(const BRDFLUTPass &other) = delete;
  BRDFLUTPass &operator=(const BRDFLUTPass &other) = delete;

  void execute(uint32_t lutSize);

  const Texture& getBRDFLUT() const { return *m_brdfLUT; }

private:
  RenderContext &m_renderContext;
  
  std::unique_ptr<sg::Primitive> m_primitive;
  std::unique_ptr<GraphicsPipeline> m_pipeline;
  std::unique_ptr<Texture> m_brdfLUT;
  std::unique_ptr<Texture> m_depthTexture;
};

} // namespace paimon
