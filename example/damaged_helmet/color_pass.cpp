#include "color_pass.h"

#include <glad/gl.h>

#include "paimon/app/application.h"
#include "paimon/core/ecs/components.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/core/world.h"
#include "paimon/rendering/render_context.h"

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
  auto &shaderManager = Application::getInstance().getShaderManager();

  auto *vertex_program =
      shaderManager.createShaderProgram("damaged_helmet.vert");
  auto *fragment_program =
      shaderManager.createShaderProgram("damaged_helmet.frag");

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
  pipelineInfo.state.vertexInput.bindings = sg::Primitive::bindings();
  pipelineInfo.state.vertexInput.attributes = sg::Primitive::attributes();

  m_pipeline = std::make_unique<GraphicsPipeline>(pipelineInfo);
  if (!m_pipeline->validate()) {
    LOG_ERROR("Failed to validate graphics pipeline");
  }

  m_color_texture = std::make_unique<Texture>(GL_TEXTURE_2D);
  m_depth_texture = std::make_unique<Texture>(GL_TEXTURE_2D);

  // Create uniform buffers
  m_transform_ubo.set_storage(sizeof(TransformUBO), nullptr,
                              GL_DYNAMIC_STORAGE_BIT);
  m_camera_ubo.set_storage(sizeof(CameraUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  m_lighting_ubo.set_storage(sizeof(LightingUBO), nullptr,
                             GL_DYNAMIC_STORAGE_BIT);
  m_material_ubo.set_storage(sizeof(MaterialUBO), nullptr,
                             GL_DYNAMIC_STORAGE_BIT);

  // Bind UBOs once for all meshes
  m_transform_ubo.bind_base(GL_UNIFORM_BUFFER, 0);
  m_camera_ubo.bind_base(GL_UNIFORM_BUFFER, 1);
  m_lighting_ubo.bind_base(GL_UNIFORM_BUFFER, 2);
  m_material_ubo.bind_base(GL_UNIFORM_BUFFER, 3);
}

void ColorPass::draw(RenderContext &ctx, const glm::ivec2 &resolution,
                     ecs::Scene &scene) {
  // Update GlobalTransform for all entities (DFS order guaranteed by entity
  // creation) Transform uses TRS (easy to edit), GlobalTransform uses Matrix
  // (efficient for rendering)
  {
    auto view = scene.view<ecs::Transform, ecs::GlobalTransform>();
    for (auto [entity, local, global] : view.each()) {

      auto *parentComp = scene.getRegistry().try_get<ecs::Parent>(entity);
      if (parentComp && parentComp->parent.isValid()) {
        // Child: Global = Parent_Global × Local
        auto &parentGlobal =
            parentComp->parent.getComponent<ecs::GlobalTransform>();
        global.matrix = parentGlobal.matrix * local.matrix();
      } else {
        // Root: Global = Local
        global.matrix = local.matrix();
      }
    }
  }

  {
    // Get camera entity and build camera UBO
    auto entity = scene.getMainCamera();
    auto &cameraComp = entity.getComponent<ecs::Camera>();
    auto &transform = entity.getComponent<ecs::GlobalTransform>();

    // Extract position from transform matrix
    glm::vec3 position = glm::vec3(transform.matrix[3]);

    CameraUBO cameraData;
    // Use view matrix calculated by OrbitCameraController
    cameraData.view = cameraComp.view;
    cameraData.projection = cameraComp.camera->getProjection();
    cameraData.position = position;
    m_camera_ubo.set_sub_data(0, sizeof(CameraUBO), &cameraData);
  }

  {
    // Get directional light entity and build lighting UBO
    auto entity = scene.getDirectionalLight();
    auto &transform = entity.getComponent<ecs::GlobalTransform>();
    auto &light = entity.getComponent<ecs::PunctualLight>().light;

    LightingUBO lightingData;

    // For directional light, use forward direction from transform
    lightingData.color = light->color;
    lightingData.intensity = light->intensity;
    lightingData.direction = glm::normalize(
        glm::vec3(transform.matrix * glm::vec4(World::Forward, 0.0f)));
    lightingData.range = light->range;
    lightingData.position =
        glm::vec3(transform.matrix * glm::vec4(World::Origin, 1.0f));
    // TODO: light types
    // lightingData.innerConeAngle = light
    m_lighting_ubo.set_sub_data(0, sizeof(LightingUBO), &lightingData);
  }

  m_color_texture->set_storage_2d(1, GL_RGBA8, resolution.x, resolution.y);
  m_depth_texture->set_storage_2d(1, GL_DEPTH_COMPONENT32, resolution.x, resolution.y);

  {
    // Setup rendering info for FBO
    RenderingInfo renderingInfo;
    renderingInfo.renderAreaOffset = {0, 0};
    renderingInfo.renderAreaExtent = {resolution.x, resolution.y};

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
    ctx.setViewport(0, 0, resolution.x, resolution.y);

    // Iterate over entities with Primitive and Material components
    auto primitiveView = scene.view<ecs::Primitive, ecs::Material, ecs::GlobalTransform>();
    for (auto [entity, primitiveComp, materialComp, transform] : primitiveView.each()) {
      if (!primitiveComp.primitive)
        continue;

      const auto &primitive = *primitiveComp.primitive;

      // Update transform uniform buffer
      TransformUBO transformData;
      transformData.model = transform.matrix;
      m_transform_ubo.set_sub_data(0, sizeof(TransformUBO), &transformData);

      // Bind vertex buffers
      if (primitive.positions) {
        ctx.bindVertexBuffer(0, *primitive.positions, 0, sizeof(glm::vec3));
      }
      if (primitive.normals) {
        ctx.bindVertexBuffer(1, *primitive.normals, 0, sizeof(glm::vec3));
      }
      if (primitive.texcoords) {
        ctx.bindVertexBuffer(2, *primitive.texcoords, 0, sizeof(glm::vec2));
      }
      if (primitive.colors) {
        ctx.bindVertexBuffer(3, *primitive.colors, 0, sizeof(glm::vec3));
      }

      // Bind index buffer if present
      if (primitive.indices) {
        ctx.bindIndexBuffer(*primitive.indices, primitive.indexType);
      }

      // Update material UBO and bind textures from Material component
      if (materialComp.material) {
        const auto &mat = materialComp.material;
        const auto &pbr = mat->pbrMetallicRoughness;

        // Prepare material data
        MaterialUBO materialData;
        materialData.baseColorFactor = pbr.baseColorFactor;
        materialData.emissiveFactor = mat->emissiveFactor;
        materialData.metallicFactor = pbr.metallicFactor;
        materialData.roughnessFactor = pbr.roughnessFactor;
        m_material_ubo.set_sub_data(0, sizeof(MaterialUBO), &materialData);

        // Bind textures with sampler
        if (pbr.baseColorTexture && pbr.baseColorTexture->image) {
          ctx.bindTexture(0, *pbr.baseColorTexture->image, *m_sampler);
        }
        if (pbr.metallicRoughnessTexture &&
            pbr.metallicRoughnessTexture->image) {
          ctx.bindTexture(1, *pbr.metallicRoughnessTexture->image,
                          *m_sampler);
        }
        if (mat->normalTexture && mat->normalTexture->image) {
          ctx.bindTexture(2, *mat->normalTexture->image, *m_sampler);
        }
        if (mat->emissiveTexture && mat->emissiveTexture->image) {
          ctx.bindTexture(3, *mat->emissiveTexture->image, *m_sampler);
        }
        if (mat->occlusionTexture && mat->occlusionTexture->image) {
          ctx.bindTexture(4, *mat->occlusionTexture->image, *m_sampler);
        }
      }

      ctx.bindUniformBuffer(0, m_transform_ubo);
      ctx.bindUniformBuffer(1, m_camera_ubo);
      ctx.bindUniformBuffer(2, m_lighting_ubo);
      ctx.bindUniformBuffer(3, m_material_ubo);

      // Draw the primitive
      if (primitive.hasIndices()) {
        ctx.drawElements(primitive.indexCount, nullptr);
      } else if (primitive.positions) {
        ctx.drawArrays(0, primitive.vertexCount);
      }
    }

    // End rendering to FBO
    ctx.endRendering();
  }
}