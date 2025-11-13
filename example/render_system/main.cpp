#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <memory>

#include "paimon/core/ecs/components.h"
#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/world.h"
#include "paimon/core/io/file.h"
#include "paimon/core/io/gltf.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/platform/window.h"
#include "paimon/rendering/render_system.h"
#include "paimon/rendering/shader_preprocessor.h"
#include "paimon/rendering/shader_source.h"

using namespace paimon;

namespace {

float g_deltaTime = 0.0f;
float g_lastFrame = 0.0f;

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
  LOG_INFO("Starting Render System Example");

  // Create window
  auto window = Window::create(WindowConfig{
      .title = "Paimon - Render System Example",
      .format = ContextFormat{
          .majorVersion = 4,
          .minorVersion = 6,
          .profile = ContextProfile::Core,
          .debug = false,
      },
      .width = 1280,
      .height = 720,
      .resizable = true,
      .visible = true,
      .vsync = true,
  });

  if (!window) {
    LOG_ERROR("Failed to create window");
    return -1;
  }

  // Load glTF model
  GltfLoader loader;
  sg::Scene scene;

  auto asset_path = std::filesystem::current_path().parent_path() / "asset" / "model";
  std::string model_path = (asset_path / "DamagedHelmet" / "glTF" / "DamagedHelmet.gltf").string();
  if (!loader.LoadFromFile(model_path, scene)) {
    LOG_ERROR("Failed to load glTF model: {}", loader.GetError());
    return -1;
  }

  if (!loader.GetWarning().empty()) {
    LOG_WARN("glTF warnings: {}", loader.GetWarning());
  }

  LOG_INFO("Loaded glTF model successfully");
  LOG_INFO("Meshes: {}, Materials: {}, Textures: {}, Images: {}",
           scene.meshes.size(), scene.materials.size(), scene.textures.size(),
           scene.images.size());

  // Create ECS world
  ecs::World world;

  // Create resource manager and load scene
  auto resource_manager = std::make_shared<rendering::ResourceManager>();
  resource_manager->LoadFromScene(scene);

  // Create render system
  rendering::RenderSystem render_system;
  render_system.Initialize(resource_manager);

  // Create entities from scene
  // For simplicity, create one entity per mesh
  for (size_t i = 0; i < scene.meshes.size(); ++i) {
    auto entity = world.createEntity();

    // Add components
    entity.addComponent<ecs::TransformComponent>();
    entity.addComponent<ecs::MeshComponent>();

    // Set mesh handle (simplified - in real implementation, need proper mapping)
    auto& mesh_comp = entity.getComponent<ecs::MeshComponent>();
    mesh_comp.meshHandle = i + 1; // Assuming handles start from 1
    mesh_comp.materialHandle = i + 1;
  }

  // Create camera entity
  auto camera_entity = world.createEntity();
  camera_entity.addComponent<ecs::TransformComponent>();
  camera_entity.addComponent<ecs::CameraComponent>();

  auto& camera_transform = camera_entity.getComponent<ecs::TransformComponent>();
  camera_transform.position = glm::vec3(0.0f, 0.0f, 3.0f);

  // Main loop
  while (!window->shouldClose()) {
    float currentFrame = glfwGetTime();
    g_deltaTime = currentFrame - g_lastFrame;
    g_lastFrame = currentFrame;

    // Handle input
    window->pollEvents();

    // Update camera (simple rotation)
    static float rotation = 0.0f;
    rotation += g_deltaTime * 0.5f;

    auto& camera_transform = camera_entity.getComponent<ecs::TransformComponent>();
    camera_transform.rotation = glm::angleAxis(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    // Clear screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Render
    render_system.Render(world);

    // Swap buffers
    window->swapBuffers();
  }

  return 0;
}