#include "paimon/utility/brdf_lut_pass.h"

#include <glad/gl.h>

#include "paimon/app/application.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

using namespace paimon;

BRDFLUTPass::BRDFLUTPass(RenderContext &renderContext)
    : m_renderContext(renderContext) {
  
  // Create quad primitive
  m_primitive = sg::Primitive::createQuad();
  
  // Get shader programs
  auto &shaderManager = Application::getInstance().getShaderManager();
  auto *vertex_program = shaderManager.createShaderProgram("ibl_brdf_lut.vert");
  auto *fragment_program = shaderManager.createShaderProgram("ibl_brdf_lut.frag");
  
  if (!vertex_program || !fragment_program) {
    LOG_ERROR("Failed to load BRDF LUT shader programs");
    return;
  }
  
  // Create graphics pipeline
  GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo.shaderStages = {
    {GL_VERTEX_SHADER_BIT, vertex_program},
    {GL_FRAGMENT_SHADER_BIT, fragment_program},
  };
  
  // Configure depth testing
  pipelineInfo.state.depthStencil.depthTestEnable = true;
  pipelineInfo.state.depthStencil.depthWriteEnable = true;
  pipelineInfo.state.depthStencil.depthCompareOp = GL_LEQUAL;
  
  // Configure vertex input
  pipelineInfo.state.vertexInput.bindings = sg::Primitive::bindings();
  pipelineInfo.state.vertexInput.attributes = sg::Primitive::attributes();
  
  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
  
  if (!m_pipeline->validate()) {
    LOG_ERROR("Failed to validate BRDF LUT pipeline");
  }

  m_brdfLUT = std::make_unique<Texture>(GL_TEXTURE_2D);
  m_depthTexture = std::make_unique<Texture>(GL_TEXTURE_2D);
}

BRDFLUTPass::~BRDFLUTPass() = default;

void BRDFLUTPass::execute(uint32_t lutSize) {
  
  // Create output BRDF LUT texture
  m_brdfLUT->set_storage_2d(1, GL_RG32F, lutSize, lutSize);
  
  // Create depth texture
  m_depthTexture->set_storage_2d(1, GL_DEPTH_COMPONENT24, lutSize, lutSize);
  
  // Setup rendering info
  RenderingInfo renderingInfo;
  renderingInfo.renderAreaOffset = {0, 0};
  renderingInfo.renderAreaExtent = {static_cast<int>(lutSize), static_cast<int>(lutSize)};
  renderingInfo.colorAttachments.emplace_back(
      *m_brdfLUT, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
      ClearValue::Color(0.0f, 0.0f, 0.0f, 1.0f));
  renderingInfo.depthAttachment.emplace(
      *m_depthTexture, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
      ClearValue::DepthStencil(1.0f, 0));
  
  m_renderContext.beginRendering(renderingInfo);
  m_renderContext.bindPipeline(*m_pipeline);
  
  // Bind vertex buffers
  if (m_primitive->positions) {
    m_renderContext.bindVertexBuffer(0, *m_primitive->positions, 0, sizeof(glm::vec3));
  }
  if (m_primitive->texcoords) {
    m_renderContext.bindVertexBuffer(2, *m_primitive->texcoords, 0, sizeof(glm::vec2));
  }
  
  // Bind index buffer if present
  if (m_primitive->hasIndices()) {
    m_renderContext.bindIndexBuffer(*m_primitive->indices, m_primitive->indexType);
  }

  if (m_primitive->hasIndices()) {
    m_renderContext.drawElements(m_primitive->indexCount, nullptr);
  } else {
    m_renderContext.drawArrays(0, m_primitive->vertexCount);
  }
  
  m_renderContext.endRendering();
}

