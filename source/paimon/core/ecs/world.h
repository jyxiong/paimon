#pragma once

#include <entt/entt.hpp>

namespace paimon {
namespace ecs {

class Entity;

class World {
public:
  World() = default;
  ~World() = default;

  World(const World &) = delete;
  World &operator=(const World &) = delete;

  // Create and destroy entities
  Entity createEntity();
  void destroyEntity(Entity entity);

  // Access to the registry
  entt::registry &getRegistry();
  const entt::registry &getRegistry() const;

  // View and iteration
  template <typename... Components>
  auto view() {
    return mRegistry.view<Components...>();
  }

  template <typename... Components>
  auto view() const {
    return mRegistry.view<Components...>();
  }

  // Group
  template <typename... Owned, typename... Get, typename... Exclude>
  auto group(entt::get_t<Get...> = {}, entt::exclude_t<Exclude...> = {}) {
    return mRegistry.group<Owned...>(entt::get<Get...>, entt::exclude<Exclude...>);
  }

  // Clear all entities
  void clear();

  // Check if entity is valid
  bool valid(entt::entity entity) const;

private:
  entt::registry mRegistry;

  friend class Entity;
};

} // namespace ecs
} // namespace paimon
