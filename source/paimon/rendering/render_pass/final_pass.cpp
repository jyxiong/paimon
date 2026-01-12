#include "paimon/rendering/render_pass/final_pass.h"

#include <glad/gl.h>

#include "paimon/app/application.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/render_context.h"

using namespace paimon;

FinalPass::FinalPass(RenderContext &renderContext)
    : m_renderContext(renderContext) {
  // Create a minimal VAO (no vertex data needed, vertices are in shader)
  // m_vao = std::make_unique<VertexArray>();

  // Setup sampler for texture filtering
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Load shaders from Application's ShaderManager
  auto &shaderManager = Application::getInstance().getShaderManager();
  auto *vert_program = shaderManager.createShaderProgram("screen_quad.vert");
  auto *frag_program = shaderManager.createShaderProgram("screen_quad.frag");

  if (!vert_program || !frag_program) {
    LOG_ERROR("Failed to load screen quad shaders");
    return;
  }

  // Create graphics pipeline
  GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo.shaderStages = {
      {GL_VERTEX_SHADER_BIT, vert_program},
      {GL_FRAGMENT_SHADER_BIT, frag_program},
  };
  pipelineInfo.state.depthStencil.depthTestEnable = false;
  
  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
}

void FinalPass::draw(RenderContext& ctx, const Texture &texture, const glm::ivec2 &size) {

  SwapchainRenderingInfo renderingInfo;
  renderingInfo.renderAreaOffset = {0, 0};
  renderingInfo.renderAreaExtent = {size.x, size.y};
  renderingInfo.clearColor = ClearValue::Color(0.0f, 0.0f, 0.0f, 1.0f);
  renderingInfo.clearDepth = 1.0f;
  renderingInfo.clearStencil = 0;

  ctx.beginSwapchainRendering(renderingInfo);

  ctx.bindPipeline(*m_pipeline);
  
  ctx.bindTexture(6, texture, *m_sampler);

  ctx.drawArrays(0, 6);

  ctx.endSwapchainRendering();
}
