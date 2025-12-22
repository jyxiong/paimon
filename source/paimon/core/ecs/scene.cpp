#include "paimon/core/ecs/scene.h"

#include "paimon/core/ecs/components.h"

namespace paimon {
namespace ecs {

Scene::Scene() {
  // Initialize main camera entity
  m_mainCamera = createEntity();
  m_mainCamera.addComponent<Name>("MainCamera");
  m_mainCamera.addComponent<Camera>();
  m_mainCamera.addComponent<GlobalTransform>();

  // Initialize directional light entity
  m_directionalLight = createEntity();
  m_directionalLight.addComponent<Name>("DirectionalLight");
  m_directionalLight.addComponent<GlobalTransform>();
  m_directionalLight.addComponent<PunctualLight>();
}

Entity Scene::createEntity() {
  return Entity{ this, m_registry.create() };
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

} // namespace ecs
} // namespace paimon
