#include "paimon/core/ecs/entity.h"

#include "paimon/core/ecs/scene.h"

namespace paimon {
namespace ecs {

Entity::Entity(Scene *scene, entt::entity handle)
    : m_scene(scene), m_handle(handle) {}

entt::registry &Entity::getRegistry() { return m_scene->getRegistry(); }

const entt::registry &Entity::getRegistry() const { return m_scene->getRegistry(); }

bool Entity::isValid() const {
  return m_scene != nullptr && m_scene->getRegistry().valid(m_handle);
}

Entity::operator bool() const { return isValid(); }

bool Entity::operator==(const Entity &other) const {
  return m_handle == other.m_handle && m_scene == other.m_scene;
}

bool Entity::operator!=(const Entity &other) const { return !(*this == other); }

entt::entity Entity::getHandle() const { return m_handle; }

Scene *Entity::getScene() const { return m_scene; }

} // namespace ecs
} // namespace paimon
