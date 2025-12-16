#include "color_pass.h"

#include <glad/gl.h>

#include "paimon/core/log_system.h"
#include "paimon/rendering/render_context.h"
#include "paimon/rendering/shader_manager.h"

using namespace paimon;

ColorPass::ColorPass(RenderContext &renderContext)
    : m_renderContext(renderContext) {
  // Create a minimal VAO (no vertex data needed, vertices are in shader)
  // m_vao = std::make_unique<VertexArray>();

  // Setup sampler for texture filtering
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Get shader programs for main rendering (separable programs for pipeline)
  auto &shaderManager = ShaderManager::getInstance();

  auto *vertex_program = m_renderContext.createShaderProgram(
      shaderManager.getShaderSource("damaged_helmet.vert"));
  auto *fragment_program = m_renderContext.createShaderProgram(
      shaderManager.getShaderSource("damaged_helmet.frag"));

  if (!vertex_program || !fragment_program) {
    LOG_ERROR("Failed to load main shader programs");
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
  pipelineInfo.state.depthStencil.depthCompareOp = GL_LESS;

  // Disable face culling for debugging
  pipelineInfo.state.rasterization.cullMode = GL_BACK;

  // VertexInputState - configure vertex attribute layout
  // We use separate bindings for each attribute to allow independent buffer
  // binding
  pipelineInfo.state.vertexInput.bindings = {
      {.binding = 0, .stride = sizeof(glm::vec3)}, // Position
      {.binding = 1, .stride = sizeof(glm::vec3)}, // Normal
      {.binding = 2, .stride = sizeof(glm::vec2)}, // TexCoord
  };
  pipelineInfo.state.vertexInput.attributes = {
      {
          .location = 0,
          .binding = 0,
          .format = GL_FLOAT,
          .size = 3, // vec3
          .offset = 0,
      },
      {
          .location = 1,
          .binding = 1,
          .format = GL_FLOAT,
          .size = 3, // vec3
          .offset = 0,
      },
      {
          .location = 2,
          .binding = 2,
          .format = GL_FLOAT,
          .size = 2, // vec2
          .offset = 0,
      },
  };

  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
  if (!m_pipeline->validate()) {
    LOG_ERROR("Failed to validate graphics pipeline");
  }

  m_color_texture = std::make_unique<Texture>(GL_TEXTURE_2D);
  m_depth_texture = std::make_unique<Texture>(GL_TEXTURE_2D);
}

void ColorPass::draw(
    RenderContext &ctx, const glm::ivec2 &g_size,
    const std::vector<MeshData> &mesh_data_list,
    const std::map<std::shared_ptr<sg::Texture>, Texture *> &texturePtrMap,
    Buffer &transform_ubo, Buffer &material_ubo, Buffer &lighting_ubo) {

    m_color_texture->set_storage_2d(1, GL_RGBA8, g_size.x, g_size.y);
    m_depth_texture->set_storage_2d(1, GL_DEPTH_COMPONENT32, g_size.x, g_size.y);

    {
      // Setup rendering info for FBO
      RenderingInfo renderingInfo;
      renderingInfo.renderAreaOffset = {0, 0};
      renderingInfo.renderAreaExtent = {g_size.x, g_size.y};

      // Setup color attachment
      renderingInfo.colorAttachments.emplace_back(
          *m_color_texture, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
          ClearValue::Color(0.1f, 0.1f, 0.1f, 1.0f));

      // Setup depth attachment
      renderingInfo.depthAttachment.emplace(
          *m_depth_texture, AttachmentLoadOp::Clear, AttachmentStoreOp::Store,
          ClearValue::DepthStencil(1.0f, 0));

      // Begin rendering to FBO
      ctx.beginRendering(renderingInfo);

      // Bind pipeline (this applies depth test and other states)
      ctx.bindPipeline(*m_pipeline);

      // Set viewport
      ctx.setViewport(0, 0, g_size.x, g_size.y);

      // Render each mesh
      int meshCount = 0;
      for (const auto &mesh_data : mesh_data_list) {

        ctx.bindVertexBuffer(0, mesh_data.position_buffer, 0,
                             sizeof(glm::vec3));
        ctx.bindVertexBuffer(1, mesh_data.normal_buffer, 0, sizeof(glm::vec3));
        ctx.bindVertexBuffer(2, mesh_data.texcoord_buffer, 0,
                             sizeof(glm::vec2));
        ctx.bindIndexBuffer(mesh_data.index_buffer, GL_UNSIGNED_INT);

        // Update material UBO and bind textures
        if (mesh_data.material) {
          const auto &mat = mesh_data.material;
          const auto &pbr = mat->pbr_metallic_roughness;

          // Prepare material data
          MaterialUBO materialData;
          materialData.baseColorFactor = pbr.base_color_factor;
          materialData.emissiveFactor = mat->emissive_factor;
          materialData.metallicFactor = pbr.metallic_factor;
          materialData.roughnessFactor = pbr.roughness_factor;
          material_ubo.set_sub_data(0, sizeof(MaterialUBO), &materialData);

          transform_ubo.bind_base(GL_UNIFORM_BUFFER, 0);
          material_ubo.bind_base(GL_UNIFORM_BUFFER, 1);
          lighting_ubo.bind_base(GL_UNIFORM_BUFFER, 2);

          // Bind textures with sampler
          // Base color (unit 0)
          ctx.bindTexture(0, *texturePtrMap.at(pbr.base_color_texture),
                          *m_sampler);

          // Metallic roughness (unit 1)

          ctx.bindTexture(1, *texturePtrMap.at(pbr.metallic_roughness_texture),
                          *m_sampler);

          // Normal (unit 2)
          ctx.bindTexture(2, *texturePtrMap.at(mat->normal_texture), *m_sampler);

          // Emissive (unit 3)
          ctx.bindTexture(3, *texturePtrMap.at(mat->emissive_texture), *m_sampler);

          // Occlusion (unit 4)
          ctx.bindTexture(4, *texturePtrMap.at(mat->occlusion_texture),
                          *m_sampler);
        }

        // Draw indexed
        if (mesh_data.index_count > 0) {
          ctx.drawElements(mesh_data.index_count, nullptr);
          meshCount++;
        }
      }

      // End rendering to FBO
      ctx.endRendering();
    }
  }
