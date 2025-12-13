#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "paimon/app/window.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/shader_manager.h"

#include "renderer.h"

using namespace paimon;

int main() {
  LogSystem::init();
  LOG_INFO("=== DamagedHelmet PBR Example ===");
  LOG_INFO("Workflow:");
  LOG_INFO("  - Load glTF model with PBR materials");
  LOG_INFO("  - Use UBOs for uniforms (Transform, Material, Lighting)");
  LOG_INFO("  - Use GraphicsPipeline + RenderContext for rendering");
  LOG_INFO("");

  // Window dimensions
  glm::ivec2 windowSize = {1280, 720};

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
      .width = static_cast<uint32_t>(windowSize.x),
      .height = static_cast<uint32_t>(windowSize.y),
      .resizable = true,
      .visible = true,
      .vsync = true,
  });

  // Get asset path
  auto assetPath = std::filesystem::current_path().parent_path().parent_path() / "asset";

  // Create and initialize renderer
  Renderer renderer;
  if (!renderer.initialize(windowSize, assetPath)) {
    LOG_ERROR("Failed to initialize renderer");
    return -1;
  }

  // Load glTF model
  auto modelPath = assetPath / "model" / "DamagedHelmet/glTF/DamagedHelmet.gltf";
  if (!renderer.loadModel(modelPath.string())) {
    LOG_ERROR("Failed to load model");
    return -1;
  }

  // Setup camera
  Camera camera;
  camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
  camera.fov = 45.0f;
  renderer.setCamera(camera);

  // Setup lighting
  renderer.setLightPosition(glm::vec3(5.0f, 5.0f, 5.0f));

  LOG_INFO("Setup complete, entering render loop");

  float lastFrame = 0.0f;

  // Main render loop
  while (!window->shouldClose()) {
    // Time
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Input
    window->pollEvents();

    // Render frame
    renderer.render(deltaTime);

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Shutting down");

  // Clear shader resources before OpenGL context is destroyed
  ShaderManager::getInstance().clear();

  return 0;
}
