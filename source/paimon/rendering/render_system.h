#pragma once

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "paimon/core/ecs/world.h"
#include "paimon/core/sg/scene.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/sampler.h"

namespace paimon {
namespace rendering {

// Resource handles
using MeshHandle = uint32_t;
using MaterialHandle = uint32_t;
using TextureHandle = uint32_t;

// GPU resource for mesh
struct GPUMesh {
  VertexArray vao;
  Buffer position_buffer;
  Buffer normal_buffer;
  Buffer texcoord_buffer;
  Buffer index_buffer;
  size_t index_count = 0;
  GLenum mode = GL_TRIANGLES;
};

// GPU resource for material
struct GPUMaterial {
  std::shared_ptr<Texture> base_color_texture;
  std::shared_ptr<Texture> normal_texture;
  std::shared_ptr<Texture> metallic_roughness_texture;
  std::shared_ptr<Texture> occlusion_texture;
  std::shared_ptr<Texture> emissive_texture;

  glm::vec4 base_color_factor = glm::vec4(1.0f);
  float metallic_factor = 1.0f;
  float roughness_factor = 1.0f;
  glm::vec3 emissive_factor = glm::vec3(0.0f);
};

// Resource manager for GPU resources
class ResourceManager {
public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  // Load resources from scene graph
  void LoadFrom_scene(const sg::Scene& scene);

  // Get handles
  MeshHandle GetMeshHandle(const std::shared_ptr<sg::Mesh>& mesh) const;
  MaterialHandle GetMaterialHandle(const std::shared_ptr<sg::Material>& material) const;
  TextureHandle GetTextureHandle(const std::shared_ptr<sg::Texture>& texture) const;

  // Get GPU resources
  const GPUMesh* GetGPUMesh(MeshHandle handle) const;
  const GPUMaterial* GetGPUMaterial(MaterialHandle handle) const;
  const std::shared_ptr<Texture>& GetGPUTexture(TextureHandle handle) const;

private:
  std::unordered_map<MeshHandle, std::unique_ptr<GPUMesh>> gpu_meshes_;
  std::unordered_map<MaterialHandle, std::unique_ptr<GPUMaterial>> gpu_materials_;
  std::unordered_map<TextureHandle, std::shared_ptr<Texture>> gpu_textures_;

  // Handle generators
  MeshHandle next_mesh_handle_ = 1;
  MaterialHandle next_material_handle_ = 1;
  TextureHandle next_texture_handle_ = 1;

  // Helper functions
  std::shared_ptr<Texture> CreateTextureFromImage(const std::shared_ptr<sg::Image>& image);
  void ProcessMesh(const std::shared_ptr<sg::Mesh>& mesh);
  void ProcessMaterial(const std::shared_ptr<sg::Material>& material);
  void ProcessTexture(const std::shared_ptr<sg::Texture>& texture);
};

// Render system
class RenderSystem {
public:
  RenderSystem();
  ~RenderSystem() = default;

  // Initialize with resource manager
  void Initialize(std::shared_ptr<ResourceManager> resource_manager);

  // Render the scene using the primary camera
  void Render(ecs::World& world);

private:
  std::shared_ptr<ResourceManager> resource_manager_;
  Program program_;
  Sampler sampler_;

  // Uniform locations
  GLint model_loc_ = -1;
  GLint view_loc_ = -1;
  GLint projection_loc_ = -1;
  GLint base_color_texture_loc_ = -1;
  GLint normal_texture_loc_ = -1;
  GLint metallic_roughness_texture_loc_ = -1;
  GLint occlusion_texture_loc_ = -1;
  GLint emissive_texture_loc_ = -1;
  GLint base_color_factor_loc_ = -1;
  GLint metallic_factor_loc_ = -1;
  GLint roughness_factor_loc_ = -1;
  GLint emissive_factor_loc_ = -1;

  void SetupShaders();
  void RenderMesh(const GPUMesh& gpu_mesh, const GPUMaterial& gpu_material, const glm::mat4& model_matrix);
};

} // namespace rendering
} // namespace paimon