#pragma once

#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include <memory>

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

/// Pass for generating prefiltered environment map for specular IBL
class PrefilteredMapPass {
public:
  PrefilteredMapPass(RenderContext &renderContext);
  ~PrefilteredMapPass();

  PrefilteredMapPass(const PrefilteredMapPass &other) = delete;
  PrefilteredMapPass &operator=(const PrefilteredMapPass &other) = delete;

  void execute(const Texture &envCubemap, uint32_t prefilteredSize,
               uint32_t mipLevels);

  const Texture& getPrefilteredMap() const { return *m_prefilteredMap; }

private:
  RenderContext &m_renderContext;

  std::unique_ptr<sg::Primitive> m_primitive;
  std::unique_ptr<GraphicsPipeline> m_pipeline;

  std::unique_ptr<Sampler> m_sampler;

  std::unique_ptr<Texture> m_prefilteredMap;
  std::unique_ptr<Texture> m_depthTexture;

  std::unique_ptr<Buffer> m_ubo;
  std::unique_ptr<Buffer> m_paramsUbo;
};

} // namespace paimon
