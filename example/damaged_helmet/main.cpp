#include <filesystem>
#include <map>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "paimon/app/window.h"
#include "paimon/core/io/gltf.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"
#include "paimon/rendering/rendering_info.h"
#include "paimon/rendering/shader_manager.h"

#include "color_pass.h"
#include "final_pass.h"
#include "mesh_data.h"


using namespace paimon;

namespace {

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
  auto assetPath =
      std::filesystem::current_path().parent_path().parent_path() / "asset";
  auto shaderPath = assetPath / "shader";

  auto &shaderManager = ShaderManager::getInstance();
  shaderManager.load(shaderPath);

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

  std::map<std::shared_ptr<sg::Texture>, Texture *> texturePtrMap;
  for (const auto &[key, value] : texture_map) {
    texturePtrMap[key] = value.get();
  }

  // Create UBOs
  Buffer transform_ubo;
  transform_ubo.set_storage(sizeof(TransformUBO), nullptr,
                            GL_DYNAMIC_STORAGE_BIT);

  Buffer material_ubo;
  material_ubo.set_storage(sizeof(MaterialUBO), nullptr,
                           GL_DYNAMIC_STORAGE_BIT);

  Buffer lighting_ubo;
  lighting_ubo.set_storage(sizeof(LightingUBO), nullptr,
                           GL_DYNAMIC_STORAGE_BIT);

  // Create FBO textures for color and depth attachments
  Texture fbo_color_texture(GL_TEXTURE_2D);
  fbo_color_texture.set_storage_2d(1, GL_RGBA8, g_size.x, g_size.y);

  Texture fbo_depth_texture(GL_TEXTURE_2D);
  fbo_depth_texture.set_storage_2d(1, GL_DEPTH_COMPONENT32, g_size.x, g_size.y);

  // Create screen quad (it will load shaders internally from singleton)
  ColorPass color_pass;
  FinalPass final_pass;

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
    transformData.model =
        glm::rotate(transformData.model, glm::radians(rotation),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    transformData.view = glm::lookAt(g_camera.position,
                                     glm::vec3(0.0f, 0.0f, 0.0f), g_camera.up);
    transformData.projection =
        glm::perspective(glm::radians(g_camera.fov),
                         static_cast<float>(g_size.x) / g_size.y, 0.1f, 100.0f);
    transform_ubo.set_sub_data(0, sizeof(TransformUBO), &transformData);

    // Setup lighting
    LightingUBO lightingData;
    lightingData.lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    lightingData.viewPos = g_camera.position;
    lighting_ubo.set_sub_data(0, sizeof(LightingUBO), &lightingData);

    // ===== First Pass: Render to FBO =====
    color_pass.draw(
        ctx, g_size, 
        mesh_data_list,
        texturePtrMap,
        transform_ubo,
        material_ubo,
        lighting_ubo);

    // ===== Second Pass: Render FBO texture to screen =====
    {
      // Use screen quad to render FBO texture
      final_pass.draw(ctx, *color_pass.getColorTexture(), g_size);
    }

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Shutting down");

  // Clear shader resources before OpenGL context is destroyed
  shaderManager.clear();

  return 0;
}
