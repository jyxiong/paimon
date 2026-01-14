#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/sg/material.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/core/sg/texture.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"

// Forward declarations
namespace tinygltf {
class Model;
} // namespace tinygltf

namespace paimon {

// glTF Loader class
class GltfLoader {
public:
  GltfLoader(const std::filesystem::path &filepath);
  ~GltfLoader() = default;

  void load(ecs::Scene &scene);

  const ecs::Entity &getRootEntity() const { return m_rootEntity; }

  ecs::Entity getRootEntity() { return m_rootEntity; }

private:
  void parseBuffers();
  void parseBufferViews();
  void parseAccessors();
  void parseTextures();
  void parseMaterials();
  void parseMeshes();

  void parseNode(const tinygltf::Node &node, ecs::Entity parent, ecs::Scene &scene);
  void parseScene(const tinygltf::Scene &scene, ecs::Scene &ecs_scene);

private:
  tinygltf::Model m_model;

  ecs::Entity m_rootEntity;

  std::vector<std::shared_ptr<Sampler>> m_samplers;
  std::vector<std::shared_ptr<Texture>> m_images;
  
  // Raw memory storage
  std::vector<std::vector<uint8_t>> m_buffers;      // Buffer data in memory
  std::vector<std::vector<uint8_t>> m_bufferViews; // BufferView data in memory
  std::vector<std::shared_ptr<Buffer>> m_accessors; // Accessor -> OpenGL Buffer

  std::vector<std::shared_ptr<sg::Texture>> m_textures;
  std::vector<std::shared_ptr<sg::Material>> m_materials;
  std::vector<std::shared_ptr<sg::Mesh>> m_meshes;
};

} // namespace paimon
