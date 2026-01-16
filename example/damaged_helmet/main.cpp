#include "paimon/app/application.h"
#include "paimon/core/ecs/components.h"
#include "paimon/core/log_system.h"

using namespace paimon;

class GltfViewerApp : public Application {
public:
  GltfViewerApp() : Application() {

    auto &scene = getScene();

    {
      // Initialize environment entity
      auto environment = scene.createEntity("Environment");
      environment.addComponent<ecs::Environment>();
      scene.setEnvironment(environment);
    }
  }

  ~GltfViewerApp() override = default;
};

int main() {
  LogSystem::init();

  GltfViewerApp app;
  app.run();

  return 0;
}
