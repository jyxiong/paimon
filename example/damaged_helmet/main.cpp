#include <filesystem>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "paimon/app/window.h"
#include "paimon/core/io/gltf.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/light.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/render_context.h"
#include "paimon/rendering/shader_manager.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/ecs/components.h"

#include "color_pass.h"
#include "final_pass.h"


using namespace paimon;

namespace {

glm::ivec2 g_size = {1280, 720};

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

  // Create ECS scene
  ecs::Scene scene;
  
  // Create camera entity
  auto cameraEntity = scene.getMainCamera();
  auto &cameraTransform = cameraEntity.getComponent<ecs::GlobalTransform>();
  cameraTransform.translation = glm::vec3(0.0f, 0.0f, 3.0f);
  auto &cameraComp = cameraEntity.getComponent<ecs::Camera>();
  cameraComp.camera = std::make_shared<sg::PerspectiveCamera>();

  // Create directional light entity
  auto lightEntity = scene.getDirectionalLight();
  auto &lightComp = lightEntity.getComponent<ecs::PunctualLight>();
  lightComp.light = std::make_shared<sg::DirectionalLight>();
  lightComp.light->color = glm::vec3(1.0f);
  lightComp.light->intensity = 3.0f;
  
  // Load glTF model
  GltfLoader loader(assetPath / "model/DamagedHelmet/glTF/DamagedHelmet.gltf");
  loader.load(scene);

  // Create FBO textures for color and depth attachments
  Texture fbo_color_texture(GL_TEXTURE_2D);
  fbo_color_texture.set_storage_2d(1, GL_RGBA8, g_size.x, g_size.y);

  Texture fbo_depth_texture(GL_TEXTURE_2D);
  fbo_depth_texture.set_storage_2d(1, GL_DEPTH_COMPONENT32, g_size.x, g_size.y);

  // Create render context
  RenderContext ctx;

  // Create screen quad (it will load shaders internally from singleton)
  ColorPass color_pass(ctx);
  FinalPass final_pass(ctx);


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

    // Auto-rotate the camera
    rotation += deltaTime * 30.0f; // 30 degrees per second
    
    // Calculate camera position rotating around the origin
    float radius = 3.0f;

    {
      auto &transform = cameraEntity.getComponent<ecs::Transform>();
      transform.translation.x = radius * sin(glm::radians(rotation));
      transform.translation.z = radius * cos(glm::radians(rotation));
      transform.translation.y = 0.0f;

      // Update camera matrices


      auto &cameraComp = cameraEntity.getComponent<ecs::Camera>();
      cameraComp.view = glm::lookAt(transform.translation,
                                    glm::vec3(0.0f, 0.0f, 0.0f),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
      cameraComp.projection = glm::perspective(glm::radians(45.0f),
                                              static_cast<float>(g_size.x) / g_size.y,
                                              0.1f, 100.0f);
    }

    // ===== First Pass: Render to FBO =====
    color_pass.draw(
        ctx, g_size, 
        scene);

    // ===== Second Pass: Render FBO texture to screen =====
    {
      // Use screen quad to render FBO texture
      final_pass.draw(ctx, *color_pass.getColorTexture(), g_size);
    }

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Shutting down");

  return 0;
}
