#include "paimon/app/application.h"
#include "paimon/core/log_system.h"

using namespace paimon;

class GltfViewerApp : public Application {
public:
  GltfViewerApp() : Application() {

    auto &scene = getScene();
  }

  ~GltfViewerApp() override = default;
};

int main() {
  LogSystem::init();

  GltfViewerApp app;
  app.run();

  return 0;
}
