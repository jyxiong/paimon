#pragma once

#include <entt/entt.hpp>

#include "paimon/core/ecs/world.h"

namespace paimon {
namespace ecs {
class Entity {
public:
  Entity() = default;

  Entity(World *world, entt::entity handle);

  Entity(const Entity &) = default;

  ~Entity() = default;

  // Add component
  template <typename T, typename... Args>
  T &addComponent(Args &&...args) {
    return mWorld->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
  }

  // Get component
  template <typename T>
  T &getComponent() {
    return mWorld->mRegistry.get<T>(mHandle);
  }

  template <typename T>
  const T &getComponent() const {
    return mWorld->mRegistry.get<T>(mHandle);
  }

  // Try get component (returns nullptr if not present)
  template <typename T>
  T *tryGetComponent() {
    return mWorld->mRegistry.try_get<T>(mHandle);
  }

  template <typename T>
  const T *tryGetComponent() const {
    return mWorld->mRegistry.try_get<T>(mHandle);
  }

  // Has component
  template <typename T>
  bool hasComponent() const {
    return mWorld->mRegistry.all_of<T>(mHandle);
  }

  // Has all components
  template <typename... T>
  bool hasAllComponents() const {
    return mWorld->mRegistry.all_of<T...>(mHandle);
  }

  // Has any component
  template <typename... T>
  bool hasAnyComponent() const {
    return mWorld->mRegistry.any_of<T...>(mHandle);
  }

  // Remove component
  template <typename T>
  void removeComponent() {
    mWorld->mRegistry.remove<T>(mHandle);
  }

  // Replace component (remove and add)
  template <typename T, typename... Args>
  T &replaceComponent(Args &&...args) {
    mWorld->mRegistry.replace<T>(mHandle, std::forward<Args>(args)...);
    return mWorld->mRegistry.get<T>(mHandle);
  }

  // Get or add component
  template <typename T, typename... Args>
  T &getOrAddComponent(Args &&...args) {
    return mWorld->mRegistry.get_or_emplace<T>(mHandle, std::forward<Args>(args)...);
  }

  // Validity check
  bool isValid() const;

  operator bool() const;

  bool operator==(const Entity &other) const;

  bool operator!=(const Entity &other) const;

  // Get underlying handle
  entt::entity getHandle() const;

  // Get world
  World *getWorld() const;

private:
  World *mWorld = nullptr;
  entt::entity mHandle = entt::null;
};

} // namespace ecs
} // namespace paimon