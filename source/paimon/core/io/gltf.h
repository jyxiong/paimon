#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include "paimon/core/ecs/scene.h"
#include "paimon/core/sg/light.h"
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

  void loadScene(ecs::Scene &scene, int scene_index = 0);

private:
  void parseTexture(const tinygltf::Texture &texture);
  void pasrseMaterial(const tinygltf::Material &material);
  void parseMesh(const tinygltf::Mesh &mesh);
  void parseLight(const tinygltf::Light &light);

  void processNodes(const tinygltf::Model &model, ecs::Scene &scene);
  void processScenes(const tinygltf::Model &model, ecs::Scene &scene);

  void processTextures(const tinygltf::Model &model);
  void processMaterials(const tinygltf::Model &model);
  void processMeshes(const tinygltf::Model &model);
  void processLights(const tinygltf::Model &model);

private:
  tinygltf::Model m_model;
  tinygltf::TinyGLTF m_loader;

  std::vector<std::shared_ptr<Sampler>> m_samplers;
  std::vector<std::shared_ptr<Texture>> m_images;
  std::vector<std::shared_ptr<Buffer>> m_buffers;

  std::vector<std::shared_ptr<sg::Texture>> m_textures;
  std::vector<std::shared_ptr<sg::Material>> m_materials;
  std::vector<std::shared_ptr<sg::Mesh>> m_meshes;
  std::vector<std::shared_ptr<sg::PunctualLight>> m_lights;
  
  std::unordered_map<int, entt::entity> m_node2entity;
};

} // namespace paimon
