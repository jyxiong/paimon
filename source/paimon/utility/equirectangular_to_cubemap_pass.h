#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

namespace paimon {

namespace sg {
struct Primitive;
}

struct CubemapTransformUBO {
  glm::mat4 projection;
  glm::mat4 view;
};

class EquirectangularToCubemapPass {
public:
  EquirectangularToCubemapPass(RenderContext &renderContext);
  ~EquirectangularToCubemapPass();

  void execute(const Texture &equirectangular, 
                                    uint32_t cubemapSize = 1024);

  const Texture& getCubemap() const { return *m_cubemap; }

private:
  RenderContext& m_renderContext;
  
  std::unique_ptr<sg::Primitive> m_primitive;
  std::unique_ptr<GraphicsPipeline> m_pipeline;

  std::unique_ptr<Sampler> m_sampler;

  std::unique_ptr<Texture> m_cubemap;
  std::unique_ptr<Texture> m_depthTexture;
  
  Buffer m_transform_ubo;
  
  // Pre-computed capture matrices for 6 cubemap faces
  glm::mat4 m_captureProjection;
  glm::mat4 m_captureViews[6];
};

} // namespace paimon
