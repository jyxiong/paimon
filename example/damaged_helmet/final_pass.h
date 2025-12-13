#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <memory>

#include "paimon/core/fg/frame_graph.h"
#include "paimon/opengl/sampler.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

namespace paimon {

class FinalPass {
public:
  struct Data {
    NodeId colorInput;
  };

  FinalPass(RenderContext &renderContext, const std::filesystem::path &assetPath);

  // Register FinalPass to the frame graph
  void registerPass(FrameGraph &fg, NodeId colorInput, const glm::ivec2 &size);

private:
  RenderContext &m_renderContext;
  std::filesystem::path m_assetPath;

  // Screen quad pipeline
  std::unique_ptr<GraphicsPipeline> m_pipeline;
  std::unique_ptr<Sampler> m_sampler;
};

} // namespace paimon
