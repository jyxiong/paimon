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

/// Pass for generating diffuse irradiance map from environment cubemap
class IrradianceMapPass {
public:
  IrradianceMapPass(RenderContext &renderContext);
  ~IrradianceMapPass();

  IrradianceMapPass(const IrradianceMapPass &other) = delete;
  IrradianceMapPass &operator=(const IrradianceMapPass &other) = delete;

  void execute(const Texture &envCubemap, uint32_t irradianceSize);

  const Texture& getIrradianceMap() const { return *m_irradianceMap; }

private:
  RenderContext &m_renderContext;

  std::unique_ptr<sg::Primitive> m_primitive;
  std::unique_ptr<GraphicsPipeline> m_pipeline;

  std::unique_ptr<Sampler> m_sampler;

  std::unique_ptr<Texture> m_irradianceMap;
  std::unique_ptr<Texture> m_depthTexture;

  std::unique_ptr<Buffer> m_ubo;
};

} // namespace paimon
