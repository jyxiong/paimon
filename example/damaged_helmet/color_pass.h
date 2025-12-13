#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "paimon/core/fg/frame_graph.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

#include "mesh_data.h"

namespace paimon {

// UBO structures (shared from renderer)
struct TransformUBO;
struct MaterialUBO;
struct LightingUBO;

class ColorPass {
public:
  struct Data {
    NodeId colorOutput;
    NodeId depthOutput;
  };

  ColorPass(RenderContext &renderContext, const std::filesystem::path &assetPath);

  // Register ColorPass to the frame graph
  // Returns the color output NodeId
  NodeId registerPass(
      FrameGraph &fg, 
      const glm::ivec2 &size,
      const std::vector<MeshData> &meshDataList,
      const std::map<std::shared_ptr<sg::Texture>, Texture*> &textureMap,
      Buffer *transformUBO,
      Buffer *materialUBO,
      Buffer *lightingUBO);

private:
  RenderContext &m_renderContext;
  std::filesystem::path m_assetPath;

  // Graphics pipeline
  std::unique_ptr<GraphicsPipeline> m_pipeline;
  std::unique_ptr<Sampler> m_sampler;
};

} // namespace paimon
