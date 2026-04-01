#include "paimon/core/ecs/scene.h"

#include "paimon/core/ecs/components.h"
#include "paimon/core/ecs/entity.h"
#include "paimon/core/io/gltf.h"
#include "paimon/core/io/ibl.h"
#include "paimon/config.h"
#include <algorithm>
#include <filesystem>

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
  if (!entity.isValid() || entity.getScene() != this) {
    return;
  }

  // Destroy all descendants first to avoid leaving orphan entities.
  std::vector<Entity> childrenSnapshot;
  if (entity.hasComponent<Children>()) {
    childrenSnapshot = entity.getComponent<Children>().children;
  }

  for (auto child : childrenSnapshot) {
    if (child.isValid() && child.getScene() == this && child != entity) {
      destroyEntity(child);
    }
  }

  // Remove this entity from its parent's children list.
  if (entity.hasComponent<Parent>()) {
    auto parent = entity.getComponent<Parent>().parent;
    if (parent.isValid() && parent.getScene() == this &&
        parent.hasComponent<Children>()) {
      auto &siblings = parent.getComponent<Children>().children;
      siblings.erase(std::remove(siblings.begin(), siblings.end(), entity),
                     siblings.end());
    }
  }

  if (m_mainCamera == entity) {
    m_mainCamera = Entity{};
  }
  if (m_directionalLight == entity) {
    m_directionalLight = Entity{};
  }
  if (m_environment == entity) {
    m_environment = Entity{};
  }

  m_registry.destroy(entity.getHandle());
}

entt::registry &Scene::getRegistry() { return m_registry; }

const entt::registry &Scene::getRegistry() const { return m_registry; }

void Scene::clear() { m_registry.clear(); }

bool Scene::valid(entt::entity entity) const {
  return m_registry.valid(entity);
}

void Scene::loadModel(const std::filesystem::path &filepath) {
  GltfLoader loader(filepath);
  loader.load(*this);
}

void Scene::loadEnvironment(const std::filesystem::path &filepath) {
  IBLLoader loader(filepath);
  loader.load(*this);
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
    directionalLight.addComponent<DirectionalLight>();

    auto &transform = directionalLight.getComponent<ecs::Transform>();
    transform.translation = glm::vec3(0.0f, 0.0f, 3.0f);

    scene->setDirectionalLight(directionalLight);
  }

  {
    auto envEntity = scene->createEntity("Environment");
    envEntity.addComponent<ecs::Environment>();
    scene->setEnvironment(envEntity);

    scene->loadEnvironment(std::filesystem::path(PAIMON_TEXTURE_DIR) / "belfast_sunset_puresky_2k.hdr");
  }

  return scene;
}

} // namespace ecs
} // namespace paimon
