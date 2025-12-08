#include "screen_quad.h"

#include <glad/gl.h>

#include "paimon/core/log_system.h"
#include "paimon/rendering/shader_manager.h"

ScreenQuad::ScreenQuad() {
  // Create a minimal VAO (no vertex data needed, vertices are in shader)
  // m_vao = std::make_unique<VertexArray>();

  // Setup sampler for texture filtering
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Load shaders from ShaderManager singleton
  auto& shaderManager = ShaderManager::getInstance();
  auto vert_program = shaderManager.getShaderProgram("screen_quad.vert", GL_VERTEX_SHADER);
  auto frag_program = shaderManager.getShaderProgram("screen_quad.frag", GL_FRAGMENT_SHADER);

  if (!vert_program || !frag_program) {
    LOG_ERROR("Failed to load screen quad shaders");
    return;
  }

  // Create graphics pipeline
  GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo.shaderStages = {
      {GL_VERTEX_SHADER_BIT, vert_program.get()},
      {GL_FRAGMENT_SHADER_BIT, frag_program.get()},
  };
  pipelineInfo.state.depthStencil.depthTestEnable = false;
  
  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
}

void ScreenQuad::draw(RenderContext& ctx, const Texture &texture) {
  ctx.bindPipeline(*m_pipeline);
  texture.bind(6);
  m_sampler->bind(6);
  // Draw full-screen quad with 6 vertices (2 triangles)
  ctx.draw(6, 1, 0, 0);
}
