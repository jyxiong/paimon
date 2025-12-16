#pragma once

#include <memory>

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

#include "mesh_data.h"

namespace paimon {

struct TransformUBO {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};

struct MaterialUBO {
  alignas(16) glm::vec4 baseColorFactor;
  alignas(16) glm::vec3 emissiveFactor;
  alignas(4) float metallicFactor;
  alignas(4) float roughnessFactor;
  alignas(4) float _padding[3]; // alignment
};

struct LightingUBO {
  alignas(16) glm::vec3 lightPos;
  alignas(4) float _padding1;
  alignas(16) glm::vec3 viewPos;
  alignas(4) float _padding2;
};

class ColorPass {
public:
  ColorPass(RenderContext &renderContext);

  void draw(
    RenderContext &ctx, const glm::ivec2 &g_size,
    const std::vector<MeshData> &mesh_data_list,
    const std::map<std::shared_ptr<sg::Texture>, Texture *> &texturePtrMap,
    Buffer &transform_ubo, Buffer &material_ubo, Buffer &lighting_ubo);

  Texture* getColorTexture() const { return m_color_texture.get(); }

private:
  RenderContext& m_renderContext;

  std::unique_ptr<Texture> m_color_texture;
  std::unique_ptr<Texture> m_depth_texture;

  std::unique_ptr<Sampler> m_sampler;
  std::unique_ptr<GraphicsPipeline> m_pipeline;
};

}
