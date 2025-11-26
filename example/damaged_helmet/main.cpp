#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <map>

#include "glad/gl.h"
#include "paimon/app/window.h"
#include "paimon/core/io/gltf.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/framebuffer.h"
#include "paimon/opengl/render_buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"
#include "paimon/rendering/rendering_info.h"
#include "paimon/rendering/shader_manager.h"

#include "screen_quad.h"

using namespace paimon;

namespace {

// UBO structures matching shader layout
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
  alignas(4) float _padding[3]; // alignment
};

struct LightingUBO {
  alignas(16) glm::vec3 lightPos;
  alignas(4) float _padding1;
  alignas(16) glm::vec3 viewPos;
  alignas(4) float _padding2;
};

// Camera state
struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  float yaw = -90.0f;
  float pitch = 0.0f;
  float fov = 45.0f;
};

Camera g_camera;
glm::ivec2 g_size = {1280, 720};

// Helper function to create OpenGL texture from sg::Image
Texture createTextureFromImage(const std::shared_ptr<sg::Image> &image) {
  Texture texture(GL_TEXTURE_2D);

  if (!image || image->data.empty()) {
    // Create a default 1x1 white texture
    std::vector<unsigned char> white = {255, 255, 255, 255};
    texture.set_storage_2d(1, GL_RGBA8, 1, 1);
    texture.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                             white.data());
    return texture;
  }

  GLenum internalFormat = GL_RGBA8;
  GLenum format = GL_RGBA;

  if (image->components == 1) {
    internalFormat = GL_R8;
    format = GL_RED;
  } else if (image->components == 2) {
    internalFormat = GL_RG8;
    format = GL_RG;
  } else if (image->components == 3) {
    internalFormat = GL_RGB8;
    format = GL_RGB;
  }

  texture.set_storage_2d(1, internalFormat, image->width, image->height);
  texture.set_sub_image_2d(0, 0, 0, image->width, image->height, format,
                           GL_UNSIGNED_BYTE, image->data.data());
  texture.generate_mipmap();

  return texture;
}

} // namespace

int main() {
  LogSystem::init();
  LOG_INFO("=== DamagedHelmet PBR Example ===");
  LOG_INFO("Workflow:");
  LOG_INFO("  - Load glTF model with PBR materials");
  LOG_INFO("  - Use UBOs for uniforms (Transform, Material, Lighting)");
  LOG_INFO("  - Use GraphicsPipeline + RenderContext for rendering");
  LOG_INFO("");

  // Create window
  auto window = Window::create(WindowConfig{
      .title = "Paimon - DamagedHelmet PBR",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = static_cast<uint32_t>(g_size.x),
      .height = static_cast<uint32_t>(g_size.y),
      .resizable = true,
      .visible = true,
      .vsync = true,
  });

  // Setup shader manager
  auto assetPath = std::filesystem::current_path().parent_path().parent_path()/ "asset";
  auto shaderPath = assetPath / "shader";

  auto& shaderManager = ShaderManager::getInstance();
  shaderManager.load(shaderPath);

  // Get shader programs for main rendering (separable programs for pipeline)
  auto vertex_program_ptr = shaderManager.getShaderProgram(
      "damaged_helmet.vert", GL_VERTEX_SHADER);
  auto fragment_program_ptr = shaderManager.getShaderProgram(
      "damaged_helmet.frag", GL_FRAGMENT_SHADER);

  if (!vertex_program_ptr || !fragment_program_ptr) {
    LOG_ERROR("Failed to load main shader programs");
    return -1;
  }
  // Load glTF model
  GltfLoader loader;
  sg::Scene scene;

  auto assetPModelath = assetPath / "model";

  std::string model_path =
      (assetPModelath / "DamagedHelmet/glTF/DamagedHelmet.gltf").string();
  if (!loader.LoadFromFile(model_path, scene)) {
    LOG_ERROR("Failed to load glTF model: {}", loader.GetError());
    return -1;
  }

  if (!loader.GetWarning().empty()) {
    LOG_WARN("glTF warnings: {}", loader.GetWarning());
  }

  // Create graphics pipeline
  GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo.shaderStages = {
      {GL_VERTEX_SHADER_BIT, vertex_program_ptr.get()},
      {GL_FRAGMENT_SHADER_BIT, fragment_program_ptr.get()},
  };
  
  // Configure depth testing
  pipelineInfo.state.depthStencil.depthTestEnable = true;
  // pipelineInfo.state.depthStencil.depthWriteEnable = true;
  // pipelineInfo.state.depthStencil.depthCompareOp = GL_LESS;
  
  // Disable face culling for debugging
  // pipelineInfo.state.rasterization.cullMode = GL_NONE;

  auto pipeline = GraphicsPipeline(pipelineInfo);


  // Process all meshes in the scene
  struct MeshData {
    VertexArray vao;
    Buffer position_buffer;
    Buffer normal_buffer;
    Buffer texcoord_buffer;
    Buffer index_buffer;
    size_t index_count;
    std::shared_ptr<sg::Material> material;
  };

  std::vector<MeshData> mesh_data_list;

  for (const auto &mesh : scene.meshes) {
    for (const auto &primitive : mesh->primitives) {
      MeshData mesh_data;

      // Setup buffers
      if (primitive.attributes.HasPositions()) {
        mesh_data.position_buffer.set_storage(
            sizeof(glm::vec3) * primitive.attributes.positions.size(),
            primitive.attributes.positions.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.attributes.HasNormals()) {
        mesh_data.normal_buffer.set_storage(
            sizeof(glm::vec3) * primitive.attributes.normals.size(),
            primitive.attributes.normals.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.attributes.HasTexCoords0()) {
        mesh_data.texcoord_buffer.set_storage(
            sizeof(glm::vec2) * primitive.attributes.texcoords_0.size(),
            primitive.attributes.texcoords_0.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.HasIndices()) {
        mesh_data.index_buffer.set_storage(
            sizeof(uint32_t) * primitive.indices.size(),
            primitive.indices.data(), GL_DYNAMIC_STORAGE_BIT);
        mesh_data.index_count = primitive.indices.size();
      }

      // Setup VAO
      mesh_data.vao.bind();

      // Position attribute (location 0)
      if (primitive.attributes.HasPositions()) {
        auto &binding_pos = mesh_data.vao.get_binding(0);
        binding_pos.bind_buffer(mesh_data.position_buffer, 0,
                                sizeof(glm::vec3));
        auto &attr_pos = mesh_data.vao.get_attribute(0);
        attr_pos.set_format(3, GL_FLOAT, GL_FALSE, 0);
        attr_pos.bind(binding_pos);
        attr_pos.enable();
      }

      // Normal attribute (location 1)
      if (primitive.attributes.HasNormals()) {
        auto &binding_normal = mesh_data.vao.get_binding(1);
        binding_normal.bind_buffer(mesh_data.normal_buffer, 0,
                                   sizeof(glm::vec3));
        auto &attr_normal = mesh_data.vao.get_attribute(1);
        attr_normal.set_format(3, GL_FLOAT, GL_FALSE, 0);
        attr_normal.bind(binding_normal);
        attr_normal.enable();
      }

      // Texcoord attribute (location 2)
      if (primitive.attributes.HasTexCoords0()) {
        auto &binding_texcoord = mesh_data.vao.get_binding(2);
        binding_texcoord.bind_buffer(mesh_data.texcoord_buffer, 0,
                                     sizeof(glm::vec2));
        auto &attr_texcoord = mesh_data.vao.get_attribute(2);
        attr_texcoord.set_format(2, GL_FLOAT, GL_FALSE, 0);
        attr_texcoord.bind(binding_texcoord);
        attr_texcoord.enable();
      }

      // Element buffer
      if (primitive.HasIndices()) {
        mesh_data.vao.set_element_buffer(mesh_data.index_buffer);
      }

      mesh_data.material = primitive.material;
      mesh_data_list.push_back(std::move(mesh_data));
    }
  }

  // Create textures from materials
  std::map<std::shared_ptr<sg::Texture>, std::unique_ptr<Texture>> texture_map;

  for (const auto &tex_pair : scene.textures) {
    if (tex_pair && tex_pair->image) {
      texture_map[tex_pair] =
          std::make_unique<Texture>(createTextureFromImage(tex_pair->image));
    }
  }

  // Create default textures
  Texture default_white = createTextureFromImage(nullptr);
  std::vector<unsigned char> normal_data = {128, 128, 255,
                                            255}; // default normal (0, 0, 1)
  Texture default_normal(GL_TEXTURE_2D);
  default_normal.set_storage_2d(1, GL_RGBA8, 1, 1);
  default_normal.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                                  normal_data.data());

  std::vector<unsigned char> mr_data = {
      0, 255, 0, 255}; // default: no AO, full roughness, no metallic
  Texture default_metallic_roughness(GL_TEXTURE_2D);
  default_metallic_roughness.set_storage_2d(1, GL_RGBA8, 1, 1);
  default_metallic_roughness.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA,
                                              GL_UNSIGNED_BYTE, mr_data.data());

  std::vector<unsigned char> black_data = {0, 0, 0, 255};
  Texture default_black(GL_TEXTURE_2D);
  default_black.set_storage_2d(1, GL_RGBA8, 1, 1);
  default_black.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                                 black_data.data());

  // Create sampler
  Sampler sampler;
  sampler.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  sampler.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  sampler.set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  sampler.set(GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Create UBOs
  Buffer transform_ubo;
  transform_ubo.set_storage(sizeof(TransformUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  transform_ubo.bind_base(GL_UNIFORM_BUFFER, 0);

  Buffer material_ubo;
  material_ubo.set_storage(sizeof(MaterialUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  material_ubo.bind_base(GL_UNIFORM_BUFFER, 1);

  Buffer lighting_ubo;
  lighting_ubo.set_storage(sizeof(LightingUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  lighting_ubo.bind_base(GL_UNIFORM_BUFFER, 2);

  // Create FBO with color and depth attachments
  Texture fbo_color_texture(GL_TEXTURE_2D);
  fbo_color_texture.set_storage_2d(1, GL_RGBA8, g_size.x, g_size.y);

  Renderbuffer fbo_depth_buffer;
  fbo_depth_buffer.storage(GL_DEPTH24_STENCIL8, g_size.x, g_size.y);

  Framebuffer fbo;
  fbo.attachTexture(GL_COLOR_ATTACHMENT0, &fbo_color_texture, 0);
  fbo.attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, &fbo_depth_buffer);
  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
  fbo.setDrawBuffers(1, drawBuffers);
  // fbo.setReadBuffer(GL_COLOR_ATTACHMENT0);

  if (!fbo.isComplete(GL_DRAW_FRAMEBUFFER)) {
    LOG_ERROR("Framebuffer is not complete!");
    return -1;
  }

  // Create screen quad (it will load shaders internally from singleton)
  ScreenQuad screen_quad;

  // Create render context
  RenderContext ctx;

  LOG_INFO("Setup complete, entering render loop");

  float rotation = 0.0f;
  float lastFrame = 0.0f;

  // Main render loop
  while (!window->shouldClose()) {
    // Time
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Input
    window->pollEvents();

    // Auto-rotate the model
    rotation += deltaTime * 30.0f; // 30 degrees per second

    // Setup transformation matrices
    TransformUBO transformData;
    transformData.model = glm::mat4(1.0f);
    transformData.model = glm::rotate(transformData.model, 
                                     glm::radians(rotation), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    transformData.view = glm::lookAt(g_camera.position, 
                                    glm::vec3(0.0f, 0.0f, 0.0f),
                                    g_camera.up);
    transformData.projection = glm::perspective(glm::radians(g_camera.fov),
                                               static_cast<float>(g_size.x) / g_size.y, 
                                               0.1f, 100.0f);

    // Update transform UBO
    transform_ubo.set_sub_data(0, sizeof(TransformUBO), &transformData);

    // Setup lighting
    LightingUBO lightingData;
    lightingData.lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    lightingData.viewPos = g_camera.position;

    // Update lighting UBO
    lighting_ubo.set_sub_data(0, sizeof(LightingUBO), &lightingData);

    // ===== First Pass: Render to FBO =====
    fbo.bind();
    // glViewport(0, 0, g_size.x, g_size.y);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glEnable(GL_DEPTH_TEST);

    // Setup rendering info for FBO
    RenderingInfo renderingInfo;
    renderingInfo.framebuffer = &fbo;
    renderingInfo.renderAreaOffset = {0, 0};
    renderingInfo.renderAreaExtent = {g_size.x, g_size.y};

    // Setup color attachment
    RenderingAttachmentInfo colorAttachment;
    colorAttachment.loadOp = AttachmentLoadOp::Clear;
    colorAttachment.storeOp = AttachmentStoreOp::Store;
    colorAttachment.clearValue = ClearValue::Color(0.1f, 0.1f, 0.1f, 1.0f);
    renderingInfo.colorAttachments[0] = colorAttachment;

    // Setup depth attachment
    RenderingAttachmentInfo depthAttachment;
    depthAttachment.loadOp = AttachmentLoadOp::Clear;
    depthAttachment.storeOp = AttachmentStoreOp::Store;
    depthAttachment.clearValue = ClearValue::DepthStencil(1.0f, 0);
    renderingInfo.depthAttachment = depthAttachment;

    // Begin rendering to FBO
    ctx.beginRendering(renderingInfo);

    // Bind pipeline
    ctx.bindPipeline(pipeline);

    // Set viewport
    ctx.setViewport(0, 0, g_size.x, g_size.y);

    // Render each mesh
    int meshCount = 0;
    for (const auto &mesh_data : mesh_data_list) {
      // Bind vertex array
      ctx.bindVertexArray(mesh_data.vao);

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

        // Update material UBO
        material_ubo.set_sub_data(0, sizeof(MaterialUBO), &materialData);

        // Bind textures with sampler
        // Base color (unit 0)
        if (pbr.base_color_texture &&
            texture_map.count(pbr.base_color_texture)) {
          texture_map.at(pbr.base_color_texture)->bind(0);
        } else {
          default_white.bind(0);
        }
        sampler.bind(0);

        // Metallic roughness (unit 1)
        if (pbr.metallic_roughness_texture &&
            texture_map.count(pbr.metallic_roughness_texture)) {
          texture_map.at(pbr.metallic_roughness_texture)->bind(1);
        } else {
          default_metallic_roughness.bind(1);
        }
        sampler.bind(1);

        // Normal (unit 2)
        if (mat->normal_texture && texture_map.count(mat->normal_texture)) {
          texture_map.at(mat->normal_texture)->bind(2);
        } else {
          default_normal.bind(2);
        }
        sampler.bind(2);

        // Emissive (unit 3)
        if (mat->emissive_texture && texture_map.count(mat->emissive_texture)) {
          texture_map.at(mat->emissive_texture)->bind(3);
        } else {
          default_black.bind(3);
        }
        sampler.bind(3);

        // Occlusion (unit 4)
        if (mat->occlusion_texture &&
            texture_map.count(mat->occlusion_texture)) {
          texture_map.at(mat->occlusion_texture)->bind(4);
        } else {
          default_white.bind(4);
        }
        sampler.bind(4);
      }

      // Draw indexed
      if (mesh_data.index_count > 0) {
        ctx.drawIndexed(static_cast<uint32_t>(mesh_data.index_count));
        meshCount++;
      }
    }
  
    // End rendering to FBO
    ctx.endRendering();

    // ===== Second Pass: Render FBO texture to screen =====
    Framebuffer::unbind();
    glViewport(0, 0, g_size.x, g_size.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Use screen quad to render FBO texture
    screen_quad.draw(fbo_color_texture);

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Shutting down");
  
  // Clear shader resources before OpenGL context is destroyed
  shaderManager.clear();
  
  return 0;
}
