#pragma once

#include <memory>

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"
#include "paimon/core/ecs/scene.h"

namespace paimon {

struct TransformUBO {
  alignas(16) glm::mat4 model;
};

struct CameraUBO {
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
  glm::vec3 position;
};

struct LightingUBO {
  glm::vec3 color;
  float intensity;
  glm::vec3 direction;
  float range;
  glm::vec3 position;
  float innerConeAngle;
  float outerConeAngle;
  int type;
};

struct MaterialUBO {
  alignas(16) glm::vec4 baseColorFactor;
  alignas(16) glm::vec3 emissiveFactor;
  alignas(4) float metallicFactor;
  alignas(4) float roughnessFactor;
  alignas(4) float _padding[3]; // alignment
};

class ColorPass {
public:
  ColorPass(RenderContext &renderContext);

  void draw(RenderContext &ctx, const glm::ivec2 &g_size, ecs::Scene &scene);

  Texture* getColorTexture() const { return m_color_texture.get(); }

private:
  RenderContext& m_renderContext;

  std::unique_ptr<Texture> m_color_texture;
  std::unique_ptr<Texture> m_depth_texture;

  std::unique_ptr<Sampler> m_sampler;
  std::unique_ptr<GraphicsPipeline> m_pipeline;

  // Uniform buffers
  Buffer m_transform_ubo;
  Buffer m_camera_ubo;
  Buffer m_lighting_ubo;
  Buffer m_material_ubo;
};

}
