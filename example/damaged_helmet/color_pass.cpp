#include "color_pass.h"

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/core/fg/frame_graph_texture.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/shader_manager.h"

namespace paimon {

// Forward declarations of UBO structures
struct TransformUBO {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};

struct MaterialUBO {
  alignas(16) glm::vec4 baseColorFactor;
  alignas(16) glm::vec3 emissiveFactor;
  alignas(4) float metallicFactor;
  alignas(4) float roughnessFactor;
  alignas(4) float _padding[3];
};

struct LightingUBO {
  alignas(16) glm::vec3 lightPos;
  alignas(4) float _padding1;
  alignas(16) glm::vec3 viewPos;
  alignas(4) float _padding2;
};

ColorPass::ColorPass(RenderContext &renderContext, const std::filesystem::path &assetPath)
    : m_renderContext(renderContext), m_assetPath(assetPath) {
  
  auto shaderPath = m_assetPath / "shader";
  auto &shaderManager = ShaderManager::getInstance();
  
  // Get shader programs
  auto vertexProgram = shaderManager.getShaderProgram(
      "damaged_helmet.vert", GL_VERTEX_SHADER);
  auto fragmentProgram = shaderManager.getShaderProgram(
      "damaged_helmet.frag", GL_FRAGMENT_SHADER);

  if (!vertexProgram || !fragmentProgram) {
    LOG_ERROR("Failed to load shader programs");
    return;
  }

  // Create graphics pipeline
  GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo.shaderStages = {
      {GL_VERTEX_SHADER_BIT, vertexProgram.get()},
      {GL_FRAGMENT_SHADER_BIT, fragmentProgram.get()},
  };

  // Configure depth testing
  pipelineInfo.state.depthStencil.depthTestEnable = true;
  pipelineInfo.state.depthStencil.depthWriteEnable = true;
  pipelineInfo.state.depthStencil.depthCompareOp = GL_LESS;

  // Disable face culling for debugging
  pipelineInfo.state.rasterization.cullMode = GL_NONE;

  // VertexInputState
  pipelineInfo.state.vertexInput.bindings = {
      {.binding = 0, .stride = sizeof(glm::vec3)},  // Position
      {.binding = 1, .stride = sizeof(glm::vec3)},  // Normal
      {.binding = 2, .stride = sizeof(glm::vec2)},  // TexCoord
  };
  pipelineInfo.state.vertexInput.attributes = {
      {.location = 0, .binding = 0, .format = GL_FLOAT, .size = 3, .offset = 0},
      {.location = 1, .binding = 1, .format = GL_FLOAT, .size = 3, .offset = 0},
      {.location = 2, .binding = 2, .format = GL_FLOAT, .size = 2, .offset = 0},
  };

  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
  if (!m_pipeline->validate()) {
    LOG_ERROR("Failed to validate graphics pipeline");
    return;
  }

  // Create sampler
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_REPEAT);

  m_colorTexture = std::make_unique<Texture>(GL_TEXTURE_2D);
  m_depthTexture = std::make_unique<Texture>(GL_TEXTURE_2D);
}

NodeId ColorPass::registerPass(
    FrameGraph &fg, 
    const glm::ivec2 &size,
    const std::vector<MeshData> &meshDataList,
    const std::map<std::shared_ptr<sg::Texture>, Texture*> &textureMap,
    Buffer *transformUBO,
    Buffer *materialUBO,
    Buffer *lightingUBO) {

  m_colorTexture->set_storage_2d(1, GL_RGBA8, size.x, size.y);
  m_depthTexture->set_storage_2d(1, GL_DEPTH_COMPONENT32, size.x, size.y);

  auto &passData = fg.create_pass<Data>(
      "ColorPass",
      [&](FrameGraph::Builder &builder, Data &data) {
        // Create color target
        FrameGraphTexture::Descriptor colorDesc;
        colorDesc.target = GL_TEXTURE_2D;
        colorDesc.width = size.x;
        colorDesc.height = size.y;
        colorDesc.format = GL_RGBA8;
        data.colorOutput = builder.create<FrameGraphTexture>("ColorTarget", colorDesc);

        // Create depth target
        FrameGraphTexture::Descriptor depthDesc;
        depthDesc.target = GL_TEXTURE_2D;
        depthDesc.width = size.x;
        depthDesc.height = size.y;
        depthDesc.format = GL_DEPTH_COMPONENT32;
        data.depthOutput = builder.create<FrameGraphTexture>("DepthTarget", depthDesc);

        // Mark as write targets
        builder.write(data.colorOutput);
        builder.write(data.depthOutput);
      },
      [&](const Data &data, FrameGraphResources &resources, void *context) {
        // Get textures from frame graph
        auto *colorTexture = fg.get<FrameGraphTexture>(data.colorOutput).getTexture();
        auto *depthTexture = fg.get<FrameGraphTexture>(data.depthOutput).getTexture();

        // Setup rendering info for FBO
        RenderingInfo renderingInfo;
        renderingInfo.renderAreaOffset = {0, 0};
        renderingInfo.renderAreaExtent = {size.x, size.y};

        // Setup color attachment
        renderingInfo.colorAttachments.emplace_back(
            *m_colorTexture,
            AttachmentLoadOp::Clear,
            AttachmentStoreOp::Store,
            ClearValue::Color(0.1f, 0.1f, 0.1f, 1.0f));

        // Setup depth attachment
        renderingInfo.depthAttachment.emplace(
            *m_depthTexture,
            AttachmentLoadOp::Clear,
            AttachmentStoreOp::Store,
            ClearValue::Depth(1.0f));

        // Begin rendering to FBO
        m_renderContext.beginRendering(renderingInfo);

        // Bind pipeline
        m_renderContext.bindPipeline(*m_pipeline);

        // Set viewport
        m_renderContext.setViewport(0, 0, size.x, size.y);

        // Bind UBOs
        if (transformUBO) {
          m_renderContext.bindUniformBuffer(0, *transformUBO, 0, sizeof(TransformUBO));
        }
        if (lightingUBO) {
          m_renderContext.bindUniformBuffer(2, *lightingUBO, 0, sizeof(LightingUBO));
        }

        // Render each mesh
        for (const auto &meshData : meshDataList) {
          m_renderContext.bindVertexBuffer(0, meshData.position_buffer, 0, sizeof(glm::vec3));
          m_renderContext.bindVertexBuffer(1, meshData.normal_buffer, 0, sizeof(glm::vec3));
          m_renderContext.bindVertexBuffer(2, meshData.texcoord_buffer, 0, sizeof(glm::vec2));
          m_renderContext.bindIndexBuffer(meshData.index_buffer, GL_UNSIGNED_INT);

          // Update material UBO and bind textures
          if (meshData.material) {
            const auto &mat = meshData.material;
            const auto &pbr = mat->pbr_metallic_roughness;

            // Prepare material data
            MaterialUBO materialData;
            materialData.baseColorFactor = pbr.base_color_factor;
            materialData.emissiveFactor = mat->emissive_factor;
            materialData.metallicFactor = pbr.metallic_factor;
            materialData.roughnessFactor = pbr.roughness_factor;

            // Update material UBO
            if (materialUBO) {
              materialUBO->set_sub_data(0, sizeof(MaterialUBO), &materialData);
            }

            // Bind textures with sampler
            // Base color (unit 0)
            Texture *tex = nullptr;
            if (pbr.base_color_texture && textureMap.count(pbr.base_color_texture)) {
              tex = textureMap.at(pbr.base_color_texture);
              m_renderContext.bindTexture(0, *tex, *m_sampler);
            }

            // Metallic roughness (unit 1)
            tex = nullptr;
            if (pbr.metallic_roughness_texture && textureMap.count(pbr.metallic_roughness_texture)) {
              tex = textureMap.at(pbr.metallic_roughness_texture);
              m_renderContext.bindTexture(1, *tex, *m_sampler);
            }

            // Normal (unit 2)
            tex = nullptr;
            if (mat->normal_texture && textureMap.count(mat->normal_texture)) {
              tex = textureMap.at(mat->normal_texture);
              m_renderContext.bindTexture(2, *tex, *m_sampler);
            }

            // Emissive (unit 3)
            tex = nullptr;
            if (mat->emissive_texture && textureMap.count(mat->emissive_texture)) {
              tex = textureMap.at(mat->emissive_texture);
              m_renderContext.bindTexture(3, *tex, *m_sampler);
            }

            // Occlusion (unit 4)
            tex = nullptr;
            if (mat->occlusion_texture && textureMap.count(mat->occlusion_texture)) {
              tex = textureMap.at(mat->occlusion_texture);
              m_renderContext.bindTexture(4, *tex, *m_sampler);
            }
          }

          // Draw indexed
          if (meshData.index_count > 0) {
            m_renderContext.drawElements(meshData.index_count, nullptr);
          }
        }

        // End rendering to FBO
        m_renderContext.endRendering();
      });

  // Return the color output NodeId
  return passData.colorOutput;
}

} // namespace paimon
