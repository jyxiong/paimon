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
  glm::mat4 model;
};

struct CameraUBO {
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec3 position;
  float _padding[1]; // alignment - vec3 needs to be aligned as vec4 in std140
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
  float _padding[2]; // alignment
};

struct MaterialUBO {
  glm::vec4 baseColorFactor;
  glm::vec3 emissiveFactor;
  float metallicFactor;
  float roughnessFactor;
  float _padding[3]; // alignment
};

class ColorPass {
public:
  ColorPass(RenderContext &renderContext);

  void draw(RenderContext &ctx, const glm::ivec2 &resolution, ecs::Scene &scene);

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