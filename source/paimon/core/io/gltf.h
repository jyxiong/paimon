#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/sg/camera.h"
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

  void load(ecs::Scene &scene);

private:
  void parseBuffers();
  void parseBufferViews();
  void parseTextures();
  void parseMaterials();
  void parseMeshes();
  void parseLights();
  void parseCameras();

  void parseNode(const tinygltf::Node &node, ecs::Entity parent, ecs::Scene &scene);
  void parseScene(const tinygltf::Scene &scene, ecs::Scene &ecs_scene);

private:
  tinygltf::Model m_model;
  tinygltf::TinyGLTF m_loader;

  std::vector<std::shared_ptr<Sampler>> m_samplers;
  std::vector<std::shared_ptr<Texture>> m_images;
  std::vector<std::shared_ptr<Buffer>> m_buffers;

  std::vector<std::shared_ptr<Buffer>> m_bufferViews;

  std::vector<std::shared_ptr<sg::Texture>> m_textures;
  std::vector<std::shared_ptr<sg::Material>> m_materials;
  std::vector<std::shared_ptr<sg::Mesh>> m_meshes;
  std::vector<std::shared_ptr<sg::PunctualLight>> m_lights;
  std::vector<std::shared_ptr<sg::Camera>> m_cameras;
};

} // namespace paimon
