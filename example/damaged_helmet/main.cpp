#include <filesystem>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "paimon/app/application.h"
#include "paimon/config.h"
#include "paimon/core/log_system.h"

#include "renderer.h"

using namespace paimon;

const glm::ivec2 initialResolution = {800, 600};

class HelmetApp : public Application {
public:
  HelmetApp() : Application() {
    auto *renderer = pushLayer(std::make_unique<Renderer>());
    renderer->loadScene(PAIMON_MODEL_DIR "/DamagedHelmet/glTF/DamagedHelmet.gltf");
    renderer->onResize(initialResolution.x, initialResolution.y);
  }
};

int main() {
  LogSystem::init();

  HelmetApp app;
  app.run();

  return 0;
}
