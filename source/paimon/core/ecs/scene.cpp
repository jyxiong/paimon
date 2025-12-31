#include "paimon/core/ecs/scene.h"

#include "paimon/core/ecs/components.h"
#include "paimon/core/io/gltf.h"

namespace paimon {
namespace ecs {

Entity Scene::createEntity(const std::string &name) {
  auto entity = Entity{this, m_registry.create()};

  entity.addComponent<Name>(
      name.empty() ? "Entity_" + std::to_string(
                                     static_cast<uint32_t>(entity.getHandle()))
                   : name);
  entity.addComponent<Transform>();
  entity.addComponent<GlobalTransform>();
  entity.addComponent<Parent>();
  entity.addComponent<Children>();

  return entity;
}

void Scene::destroyEntity(Entity entity) {
  if (entity.isValid() && entity.getScene() == this) {
    m_registry.destroy(entity.getHandle());
  }
}

entt::registry &Scene::getRegistry() { return m_registry; }

const entt::registry &Scene::getRegistry() const { return m_registry; }

void Scene::clear() { m_registry.clear(); }

bool Scene::valid(entt::entity entity) const {
  return m_registry.valid(entity);
}

Entity Scene::load(const std::filesystem::path &filepath) {
  GltfLoader loader(filepath);
  loader.load(*this);
  return loader.getRootEntity();
}

std::unique_ptr<Scene> Scene::create() {
  auto scene = std::make_unique<Scene>();

    // Initialize main camera entity
  {
    auto mainCamera = scene->createEntity("MainCamera");
    mainCamera.addComponent<Camera>(std::make_shared<sg::PerspectiveCamera>());
    
    // Set camera position to look at the origin
    auto &transform = mainCamera.getComponent<ecs::Transform>();
    transform.translation = glm::vec3(0.0f, 0.0f, 5.0f);

    scene->setMainCamera(mainCamera);
  }

  {
    // Initialize directional light entity
    auto directionalLight = scene->createEntity("DirectionalLight");
    directionalLight.addComponent<PunctualLight>(std::make_shared<sg::DirectionalLight>());

    auto &transform = directionalLight.getComponent<ecs::Transform>();
    transform.translation = glm::vec3(0.0f, 0.0f, 3.0f);

    scene->setDirectionalLight(directionalLight);
  }

  return scene;
}

} // namespace ecs
} // namespace paimon
