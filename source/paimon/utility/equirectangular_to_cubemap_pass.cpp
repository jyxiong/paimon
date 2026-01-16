#include "paimon/utility/equirectangular_to_cubemap_pass.h"

#include <cmath>
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/app/application.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/rendering/render_context.h"

using namespace paimon;

EquirectangularToCubemapPass::EquirectangularToCubemapPass(RenderContext &renderContext)
    : m_renderContext(renderContext) {
  // Create cube primitive
  m_primitive = sg::Primitive::createCube();
  
  // Get shader programs
  auto &shaderManager = Application::getInstance().getShaderManager();
  auto *vertex_program = shaderManager.createShaderProgram("ibl_cubemap.vert");
  auto *fragment_program = shaderManager.createShaderProgram("ibl_equirect_to_cubemap.frag");
  
  if (!vertex_program || !fragment_program) {
    LOG_ERROR("Failed to load equirectangular to cubemap shader programs");
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
  
  // Configure face culling
  pipelineInfo.state.rasterization.cullMode = GL_BACK;
  pipelineInfo.state.rasterization.frontFace = GL_CCW;
  
  // Configure vertex input
  pipelineInfo.state.vertexInput.bindings = sg::Primitive::bindings();
  pipelineInfo.state.vertexInput.attributes = sg::Primitive::attributes();
  
  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
  
  if (!m_pipeline->validate()) {
    LOG_ERROR("Failed to validate equirectangular to cubemap pipeline");
  }
  
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Create uniform buffer
  m_transform_ubo.set_storage(sizeof(CubemapTransformUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  
  // Pre-compute projection matrix (90 degree FOV for cubemap faces)
  m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  
  // Pre-compute view matrices for 6 cubemap faces
  m_captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // +X
  m_captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // -X
  m_captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)); // +Y
  m_captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)); // -Y
  m_captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // +Z
  m_captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // -Z

  m_cubemap = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);
  m_depthTexture = std::make_unique<Texture>(GL_TEXTURE_2D);
}

EquirectangularToCubemapPass::~EquirectangularToCubemapPass() = default;

void EquirectangularToCubemapPass::execute(
    const Texture &equirectangular, 
    uint32_t cubemapSize) {
  
  // Create output cubemap texture with mipmaps
  uint32_t maxMipLevels = static_cast<uint32_t>(std::floor(std::log2(cubemapSize))) + 1;
  m_cubemap->set_storage_2d(maxMipLevels, GL_RGB32F, cubemapSize, cubemapSize);
  
  // Create depth texture
  m_depthTexture->set_storage_2d(1, GL_DEPTH_COMPONENT32, cubemapSize, cubemapSize);
  
  // Render to each cubemap face
  for (uint32_t i = 0; i < 6; ++i) {
    // Update view matrix for this face
    CubemapTransformUBO ubo;
    ubo.projection = m_captureProjection;
    ubo.view = m_captureViews[i];
    m_transform_ubo.set_sub_data(0, sizeof(CubemapTransformUBO), &ubo);
    
    // Setup rendering info for this cubemap face
    RenderingInfo renderingInfo;
    renderingInfo.renderAreaOffset = {0, 0};
    renderingInfo.renderAreaExtent = {static_cast<int>(cubemapSize), static_cast<int>(cubemapSize)};
    renderingInfo.colorAttachments.emplace_back(
        *m_cubemap, 0, i, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
        ClearValue::Color(0.0f, 0.0f, 0.0f, 1.0f));
    renderingInfo.depthAttachment.emplace(
        *m_depthTexture, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
        ClearValue::DepthStencil(1.0f, 0));
    
    m_renderContext.beginRendering(renderingInfo);

    m_renderContext.bindPipeline(*m_pipeline);

    m_renderContext.bindVertexBuffer(0, *m_primitive->positions, 0, sizeof(glm::vec3));
    m_renderContext.bindIndexBuffer(*m_primitive->indices, m_primitive->indexType);

    m_renderContext.bindUniformBuffer(0, m_transform_ubo);
  
    m_renderContext.bindTexture(0, equirectangular, *m_sampler);

    m_renderContext.drawElements(m_primitive->indexCount, nullptr);

    m_renderContext.endRendering();
  }  
}
