#pragma once

#include <filesystem>

#include <entt/entt.hpp>
#include <memory>

#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/system.h"

namespace paimon {
namespace ecs {

class Entity;

class Scene {
public:
  Scene() = default;
  ~Scene() = default;

  Scene(const Scene &) = delete;
  Scene &operator=(const Scene &) = delete;

  // Create and destroy entities
  Entity createEntity(const std::string &name = std::string());
  void destroyEntity(Entity entity);

  // Access to the registry
  entt::registry &getRegistry();
  const entt::registry &getRegistry() const;

  void setMainCamera(Entity camera) { m_mainCamera = camera; }

  Entity getMainCamera() { return m_mainCamera; }
  const Entity &getMainCamera() const { return m_mainCamera; }

  void setDirectionalLight(Entity light) { m_directionalLight = light; }

  Entity getDirectionalLight() { return m_directionalLight; }
  const Entity &getDirectionalLight() const { return m_directionalLight; }

  // View and iteration
  template <typename... Components>
  auto view() {
    return m_registry.view<Components...>();
  }

  template <typename... Components>
  auto view() const {
    return m_registry.view<Components...>();
  }

  // Group
  template <typename... Owned, typename... Get, typename... Exclude>
  auto group(entt::get_t<Get...> = {}, entt::exclude_t<Exclude...> = {}) {
    return m_registry.group<Owned...>(entt::get<Get...>, entt::exclude<Exclude...>);
  }

  // Clear all entities
  void clear();

  // Check if entity is valid
  bool valid(entt::entity entity) const;

  Entity load(const std::filesystem::path &filepath);

  static std::unique_ptr<Scene> create();

private:
  entt::registry m_registry;

  std::vector<std::unique_ptr<System>> m_systems;

  Entity m_mainCamera;
  Entity m_directionalLight;
};

} // namespace ecs
} // namespace paimon
