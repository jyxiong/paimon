#include "final_pass.h"

#include <glad/gl.h>

#include "paimon/core/fg/frame_graph_texture.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/rendering_info.h"
#include "paimon/rendering/shader_manager.h"

namespace paimon {

FinalPass::FinalPass(RenderContext &renderContext, const std::filesystem::path &assetPath)
    : m_renderContext(renderContext), m_assetPath(assetPath) {
  
  auto shaderPath = m_assetPath / "shader";
  auto &shaderManager = ShaderManager::getInstance();
  
  // Get shader programs
  auto screenVertProgram = shaderManager.getShaderProgram(
      "screen_quad.vert", GL_VERTEX_SHADER);
  auto screenFragProgram = shaderManager.getShaderProgram(
      "screen_quad.frag", GL_FRAGMENT_SHADER);

  if (!screenVertProgram || !screenFragProgram) {
    LOG_ERROR("Failed to load screen quad shader programs");
    return;
  }

  // Create screen quad pipeline
  GraphicsPipelineCreateInfo screenPipelineInfo;
  screenPipelineInfo.shaderStages = {
      {GL_VERTEX_SHADER_BIT, screenVertProgram.get()},
      {GL_FRAGMENT_SHADER_BIT, screenFragProgram.get()},
  };
  screenPipelineInfo.state.depthStencil.depthTestEnable = false;
  screenPipelineInfo.state.depthStencil.depthWriteEnable = false;
  m_pipeline = std::make_unique<GraphicsPipeline>(screenPipelineInfo);
  
  // Create sampler
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void FinalPass::registerPass(FrameGraph &fg, NodeId colorInput, const glm::ivec2 &size) {
  
  fg.create_pass<Data>(
      "FinalPass",
      [colorInput](FrameGraph::Builder &builder, Data &data) {
        // Read color input from previous pass
        data.colorInput = colorInput;
        builder.read(data.colorInput);
      },
      [&](FrameGraphResources &resources, void *context) {
        // Get source texture from frame graph
        auto &texture = fg.get<FrameGraphTexture>(colorInput);
        
        // Setup rendering to default framebuffer
        SwapchainRenderingInfo renderingInfo;
        renderingInfo.renderAreaOffset = {0, 0};
        renderingInfo.renderAreaExtent = {size.x, size.y};
        
        // Begin rendering to default framebuffer
        m_renderContext.beginSwapchainRendering(renderingInfo);
        
        // Bind screen quad pipeline
        m_renderContext.bindPipeline(*m_pipeline);
        
        // Bind texture and sampler
        m_renderContext.bindTexture(0, *texture.getTexture(), *m_sampler);
        
        // Set viewport
        m_renderContext.setViewport(0, 0, size.x, size.y);
        
        // Draw fullscreen quad (without vertex buffers, generated in vertex shader)
        m_renderContext.drawArrays(0, 3);
        
        // End rendering
        m_renderContext.endSwapchainRendering();
      });
}

} // namespace paimon
