#include <memory>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "paimon/app/application.h"
#include "paimon/app/panel/editor_layer.h"
#include "paimon/core/log_system.h"

#include "renderer.h"

using namespace paimon;

class HelmetApp : public Application {
public:
  HelmetApp() : Application() {
    // Get default scene from application
    auto* scene = getScene();

    // Setup editor layer first
    auto* editorLayer = pushLayer(std::make_unique<EditorLayer>());

    // Setup renderer with the scene and viewport
    auto *renderer = pushLayer(std::make_unique<Renderer>());
    renderer->setViewportPanel(&editorLayer->getViewportPanel());
  }
};

int main() {
  LogSystem::init();

  HelmetApp app;
  app.run();

  return 0;
}
