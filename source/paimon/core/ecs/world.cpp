#include "paimon/core/ecs/world.h"
#include "paimon/core/ecs/entity.h"

namespace paimon {
namespace ecs {

Entity World::createEntity() {
  entt::entity handle = mRegistry.create();
  return Entity(this, handle);
}

void World::destroyEntity(Entity entity) {
  if (entity.isValid() && entity.getWorld() == this) {
    mRegistry.destroy(entity.getHandle());
  }
}

entt::registry &World::getRegistry() {
  return mRegistry;
}

const entt::registry &World::getRegistry() const {
  return mRegistry;
}

void World::clear() {
  mRegistry.clear();
}

bool World::valid(entt::entity entity) const {
  return mRegistry.valid(entity);
}

} // namespace ecs
} // namespace paimon
