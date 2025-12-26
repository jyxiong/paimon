#include <filesystem>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "paimon/app/application.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/shader_manager.h"

#include "renderer.h"

using namespace paimon;

const glm::ivec2 initialResolution = {800, 600};

int main() {
  LogSystem::init();

  // Setup shader manager
  auto assetPath =
      std::filesystem::current_path().parent_path().parent_path() / "asset";

  Application app({.windowConfig = {.title = "Triangle Example",
                                    .width = initialResolution.x,
                                    .height = initialResolution.y},
                   .assetPath = assetPath});

  auto &shaderManager = ShaderManager::getInstance();
  shaderManager.load(assetPath / "shader");

  auto *renderer = app.pushLayer(std::make_unique<Renderer>());
  renderer->loadScene(assetPath /
                      "model/DamagedHelmet/glTF/DamagedHelmet.gltf");
  renderer->onResize(initialResolution.x, initialResolution.y);

  app.run();

  return 0;
}
