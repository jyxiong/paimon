#include "paimon/core/ecs/entity.h"

namespace paimon {
namespace ecs {

Entity::Entity(World *world, entt::entity handle) 
    : mWorld(world), mHandle(handle) {
}

bool Entity::isValid() const {
  return mWorld != nullptr && mWorld->mRegistry.valid(mHandle);
}

Entity::operator bool() const {
  return isValid();
}

bool Entity::operator==(const Entity &other) const {
  return mHandle == other.mHandle && mWorld == other.mWorld;
}

bool Entity::operator!=(const Entity &other) const {
  return !(*this == other);
}

entt::entity Entity::getHandle() const {
  return mHandle;
}

World *Entity::getWorld() const {
  return mWorld;
}

} // namespace ecs
} // namespace paimon
