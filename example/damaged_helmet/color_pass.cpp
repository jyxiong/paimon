#include "color_pass.h"

#include <glad/gl.h>

#include "paimon/core/ecs/components.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/type.h"
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

void ColorPass::draw(RenderContext &ctx, const glm::ivec2 &g_size,
                     ecs::Scene &scene) {

  // ═══════════════════════════════════════════════════════════════════════════
  // GlobalTransform 更新策略详解与优化
  // ═══════════════════════════════════════════════════════════════════════════
  //
  // 【核心问题】
  // 父子变换必须按层级顺序更新：parent.global * child.local = child.global
  // 父节点的GlobalTransform必须在子节点之前计算完成
  //
  // 【优化策略】
  // 1. 使用迭代栈代替递归 - 避免std::function调用开销和栈溢出风险
  // 2. 利用EnTT的group优化内存访问局部性
  // 3. 预先收集根节点，避免重复查询
  // 4. 显式管理遍历栈，实现深度优先遍历（DFS）
  //
  // 【时间复杂度】O(n) - 每个节点访问一次
  // 【空间复杂度】O(h) - h为树的最大深度（栈空间）
  // ═══════════════════════════════════════════════════════════════════════════

  // Step 1: 使用EnTT group优化内存访问
  // Group将Transform和GlobalTransform的数据紧密排列，提高缓存命中率
  auto transformGroup = scene.getRegistry().group<ecs::GlobalTransform>(
      entt::get<ecs::Transform>);

  // Step 2: 收集所有根节点（没有父节点或父节点无效的Children实体）
  auto childrenView = scene.view<ecs::Children>();
  std::vector<entt::entity> roots;
  roots.reserve(8); // 预分配常见场景的根节点数量
  
  for (auto entity : childrenView) {
    auto *parentComp = scene.getRegistry().try_get<ecs::Parent>(entity);
    if (!parentComp || !parentComp->parent.isValid()) {
      roots.push_back(entity);
    }
  }

  // Step 3: 使用显式栈进行迭代式深度优先遍历
  // 避免递归调用开销，对深层级场景更稳定
  struct TraversalNode {
    entt::entity entity;
    glm::mat4 parentMatrix; // 父节点的GlobalTransform矩阵
  };

  std::vector<TraversalNode> stack;
  stack.reserve(64); // 预分配合理深度

  // Step 4: 遍历每个根节点的子树
  for (auto rootEntity : roots) {
    // 根节点没有父变换，使用单位矩阵
    stack.push_back({rootEntity, glm::mat4(1.0f)});

    while (!stack.empty()) {
      auto [currentEntity, parentMatrix] = stack.back();
      stack.pop_back();

      // 更新当前实体的GlobalTransform
      auto *localTransform = scene.getRegistry().try_get<ecs::Transform>(currentEntity);
      auto *globalTransform = scene.getRegistry().try_get<ecs::GlobalTransform>(currentEntity);

      glm::mat4 currentGlobalMatrix = parentMatrix;
      
      if (localTransform && globalTransform) {
        // 计算全局变换: Global = Parent_Global × Local
        auto *parentComp = scene.getRegistry().try_get<ecs::Parent>(currentEntity);
        if (parentComp && parentComp->parent.isValid()) {
          currentGlobalMatrix = parentMatrix * localTransform->matrix;
        } else {
          // 根节点：Global = Local
          currentGlobalMatrix = localTransform->matrix;
        }
        globalTransform->matrix = currentGlobalMatrix;
      }

      // 将子节点入栈（逆序入栈保证正序遍历）
      auto *childrenComp = scene.getRegistry().try_get<ecs::Children>(currentEntity);
      if (childrenComp && !childrenComp->children.empty()) {
        // 逆序入栈，使得第一个子节点先被处理
        for (auto it = childrenComp->children.rbegin(); 
             it != childrenComp->children.rend(); ++it) {
          stack.push_back({it->getHandle(), currentGlobalMatrix});
        }
      }
    }
  }

  {
    // Get camera entity and build transform UBO
    auto entity = scene.getMainCamera();
    auto &camera = entity.getComponent<ecs::Camera>().camera;
    auto &transform = entity.getComponent<ecs::GlobalTransform>();

    CameraUBO cameraData;
    cameraData.view = glm::inverse(transform.matrix);
    cameraData.projection = camera->getProjection();
    cameraData.position =
        glm::vec3(transform.matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
        glm::vec3(transform.matrix * glm::vec4(light->direction, 0.0f)));
    lightingData.range = light->range;
    lightingData.position =
        glm::vec3(transform.matrix * glm::vec4(light->position, 1.0f));
    // TODO: light types
    // lightingData.innerConeAngle = light
    m_lighting_ubo.set_sub_data(0, sizeof(LightingUBO), &lightingData);
  }

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

    // Iterate over entities with Mesh component
    auto meshView = scene.view<ecs::Mesh, ecs::GlobalTransform>();
    int meshCount = 0;

    for (auto entity : meshView) {
      auto &mesh = meshView.get<ecs::Mesh>(entity).mesh;
      auto &transform = meshView.get<ecs::GlobalTransform>(entity);

      if (!mesh)
        continue;

      // Update uniform buffers
      TransformUBO transformData;
      transformData.model = transform.matrix;
      m_transform_ubo.set_sub_data(0, sizeof(TransformUBO), &transformData);

      // Render each primitive in the mesh
      for (const auto &primitive : mesh->primitives) {
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

        // Bind index buffer if present
        if (primitive.indices) {
          ctx.bindIndexBuffer(*primitive.indices, DataType::UInt);
        }

        // Update material UBO and bind textures
        if (primitive.material) {
          const auto &mat = primitive.material;
          const auto &pbr = mat->pbrMetallicRoughness;

          // Prepare material data
          MaterialUBO materialData;
          materialData.baseColorFactor = pbr.baseColorFactor;
          materialData.emissiveFactor = mat->emissiveFactor;
          materialData.metallicFactor = pbr.metallicFactor;
          materialData.roughnessFactor = pbr.roughnessFactor;
          m_material_ubo.set_sub_data(0, sizeof(MaterialUBO), &materialData);

          // Bind textures with sampler - directly from material
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
          meshCount++;
        } else if (primitive.positions) {
          ctx.drawArrays(0, primitive.vertexCount);
          meshCount++;
        }
      }
    }

    // End rendering to FBO
    ctx.endRendering();
  }
}