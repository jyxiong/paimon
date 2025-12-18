#include "paimon/core/ecs/scene.h"

namespace paimon {
namespace ecs {

Entity Scene::createEntity() {
  entt::entity handle = m_registry.create();
  return Entity(this, handle);
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
