#include "paimon/core/ecs/scene.h"

#include "paimon/core/ecs/components.h"
#include "paimon/core/io/gltf.h"

namespace paimon {
namespace ecs {

Scene::Scene() {
  // Initialize main camera entity
  m_mainCamera = createEntity("MainCamera");
  m_mainCamera.addComponent<Camera>();

  // Initialize directional light entity
  m_directionalLight = createEntity("DirectionalLight");
  m_directionalLight.addComponent<PunctualLight>();
}

Entity Scene::createEntity(const std::string &name) {
  auto entity = Entity{ this, m_registry.create() };

  entity.addComponent<Name>(name.empty() ? "Entity_" + std::to_string(static_cast<uint32_t>(entity.getHandle())) : name);
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

entt::registry &Scene::getRegistry() {
  return m_registry;
}

const entt::registry &Scene::getRegistry() const {
  return m_registry;
}

void Scene::clear() {
  m_registry.clear();
}

bool Scene::valid(entt::entity entity) const {
  return m_registry.valid(entity);
}

Entity Scene::load(const std::filesystem::path &filepath) {
  GltfLoader loader(filepath);
  loader.load(*this);
  return loader.getRootEntity();
}

} // namespace ecs
} // namespace paimon
