#include "paimon/utility/prefiltered_map_pass.h"

#include <cmath>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/app/application.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/opengl/buffer.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"

using namespace paimon;

PrefilteredMapPass::PrefilteredMapPass(RenderContext &renderContext)
    : m_renderContext(renderContext) {

  // Create cube primitive
  m_primitive = sg::Primitive::createCube();

  // Create UBO for view-projection matrices
  m_ubo = std::make_unique<Buffer>();
  m_ubo->set_storage(sizeof(glm::mat4) * 7, nullptr, GL_DYNAMIC_STORAGE_BIT);

  // Create UBO for prefilter parameters (roughness and resolution)
  m_paramsUbo = std::make_unique<Buffer>();
  m_paramsUbo->set_storage(sizeof(float) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);

  // Get shader programs
  auto &shaderManager = Application::getInstance().getShaderManager();
  auto *vertex_program = shaderManager.createShaderProgram("ibl_cubemap.vert");
  auto *fragment_program =
      shaderManager.createShaderProgram("ibl_prefilter.frag");

  if (!vertex_program || !fragment_program) {
    LOG_ERROR("Failed to load prefiltered map shader programs");
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
    LOG_ERROR("Failed to validate prefiltered map pipeline");
  }

  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  m_prefilteredMap = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);
  m_depthTexture = std::make_unique<Texture>(GL_TEXTURE_2D);
}

PrefilteredMapPass::~PrefilteredMapPass() = default;

void PrefilteredMapPass::execute(const Texture &envCubemap,
                                 uint32_t prefilteredSize, uint32_t mipLevels) {

  // Create output prefiltered cubemap with mipmaps
  m_prefilteredMap->set_storage_2d(mipLevels, GL_RGB16F, prefilteredSize,
                                   prefilteredSize);

  // Prepare view matrices for 6 faces
  glm::mat4 captureProjection =
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  glm::mat4 captureViews[] = {
      glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f))};

  // Get environment map resolution
  GLint envMapWidth = 0;
  glGetTextureLevelParameteriv(envCubemap.get_name(), 0, GL_TEXTURE_WIDTH,
                               &envMapWidth);

  // Create depth texture (sized for largest mip level)
  m_depthTexture->set_storage_2d(1, GL_DEPTH_COMPONENT24, prefilteredSize,
                                 prefilteredSize);

  // Update parameters UBO with environment map resolution
  struct PrefilteredParams {
    float roughness;
    float envMapResolution;
  } params;
  params.envMapResolution = static_cast<float>(envMapWidth);

  // Render each mip level
  for (uint32_t mip = 0; mip < mipLevels; ++mip) {
    uint32_t mipSize =
        static_cast<uint32_t>(prefilteredSize * std::pow(0.5f, mip));
    float roughness =
        static_cast<float>(mip) / static_cast<float>(mipLevels - 1);

    // Update roughness in parameters UBO for this mip level
    params.roughness = roughness;
    m_paramsUbo->set_sub_data(0, sizeof(PrefilteredParams), &params);

    // Render each face
    for (uint32_t i = 0; i < 6; ++i) {
      // Update view matrix for this face
      struct UBO {
        glm::mat4 projection;
        glm::mat4 view;
      } ubo;
      ubo.projection = captureProjection;
      ubo.view = captureViews[i];
      m_ubo->set_sub_data(0, sizeof(UBO), &ubo);

      // Setup rendering info for this cubemap face at this mip level
      RenderingInfo renderingInfo;
      renderingInfo.renderAreaOffset = {0, 0};
      renderingInfo.renderAreaExtent = {static_cast<int>(mipSize),
                                        static_cast<int>(mipSize)};
      renderingInfo.colorAttachments.emplace_back(
          *m_prefilteredMap, mip, i, AttachmentLoadOp::Clear,
          AttachmentStoreOp::Store, ClearValue::Color(0.0f, 0.0f, 0.0f, 1.0f));
      renderingInfo.depthAttachment.emplace(
          *m_depthTexture, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
          ClearValue::DepthStencil(1.0f, 0));

      m_renderContext.beginRendering(renderingInfo);
      m_renderContext.bindPipeline(*m_pipeline);

      m_renderContext.bindTexture(0, envCubemap, *m_sampler);

      m_renderContext.bindUniformBuffer(0, *m_ubo);
      m_renderContext.bindUniformBuffer(1, *m_paramsUbo);

      m_renderContext.bindVertexBuffer(0, *m_primitive->positions, 0,
                                       sizeof(glm::vec3));

      m_renderContext.bindIndexBuffer(*m_primitive->indices,
                                      m_primitive->indexType);
      m_renderContext.drawElements(m_primitive->indexCount, nullptr);

      m_renderContext.endRendering();
    }
  }
}
