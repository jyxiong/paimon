#include "render_system.h"

#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

#include "paimon/core/ecs/components.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/image.h"
#include "paimon/core/sg/material.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/core/sg/texture.h"
#include "paimon/opengl/shader.h"

namespace paimon {
namespace rendering {

void ResourceManager::LoadFrom_scene(const sg::Scene& scene) {
  // Process textures first
  for (const auto& texture : scene.textures) {
    if (texture) {
      ProcessTexture(texture);
    }
  }

  // Process materials
  for (const auto& material : scene.materials) {
    if (material) {
      ProcessMaterial(material);
    }
  }

  // Process meshes
  for (const auto& mesh : scene.meshes) {
    if (mesh) {
      ProcessMesh(mesh);
    }
  }
}

MeshHandle ResourceManager::GetMeshHandle(const std::shared_ptr<sg::Mesh>& mesh) const {
  for (const auto& pair : gpu_meshes_) {
    // TODO: Implement proper mapping
    return pair.first;
  }
  return 0;
}

MaterialHandle ResourceManager::GetMaterialHandle(const std::shared_ptr<sg::Material>& material) const {
  for (const auto& pair : gpu_materials_) {
    // TODO: Implement proper mapping
    return pair.first;
  }
  return 0;
}

TextureHandle ResourceManager::GetTextureHandle(const std::shared_ptr<sg::Texture>& texture) const {
  for (const auto& pair : gpu_textures_) {
    // TODO: Implement proper mapping
    return pair.first;
  }
  return 0;
}

const GPUMesh* ResourceManager::GetGPUMesh(MeshHandle handle) const {
  auto it = gpu_meshes_.find(handle);
  return it != gpu_meshes_.end() ? it->second.get() : nullptr;
}

const GPUMaterial* ResourceManager::GetGPUMaterial(MaterialHandle handle) const {
  auto it = gpu_materials_.find(handle);
  return it != gpu_materials_.end() ? it->second.get() : nullptr;
}

const std::shared_ptr<Texture>& ResourceManager::GetGPUTexture(TextureHandle handle) const {
  static std::shared_ptr<Texture> null_texture;
  auto it = gpu_textures_.find(handle);
  return it != gpu_textures_.end() ? it->second : null_texture;
}

std::shared_ptr<Texture> ResourceManager::CreateTextureFromImage(const std::shared_ptr<sg::Image>& image) {
  auto texture = std::make_shared<Texture>(GL_TEXTURE_2D);

  if (!image || image->data.empty()) {
    // Create a default 1x1 white texture
    std::vector<unsigned char> white = {255, 255, 255, 255};
    texture->set_storage_2d(1, GL_RGBA8, 1, 1);
    texture->set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, white.data());
    return texture;
  }

  GLenum internalFormat = GL_RGBA8;
  GLenum format = GL_RGBA;

  if (image->components == 1) {
    internalFormat = GL_R8;
    format = GL_RED;
  } else if (image->components == 2) {
    internalFormat = GL_RG8;
    format = GL_RG;
  } else if (image->components == 3) {
    internalFormat = GL_RGB8;
    format = GL_RGB;
  }

  texture->set_storage_2d(1, internalFormat, image->width, image->height);
  texture->set_sub_image_2d(0, 0, 0, image->width, image->height, format, GL_UNSIGNED_BYTE, image->data.data());
  texture->generate_mipmap();

  return texture;
}

void ResourceManager::ProcessMesh(const std::shared_ptr<sg::Mesh>& mesh) {
  for (const auto& primitive : mesh->primitives) {
    auto gpu_mesh = std::make_unique<GPUMesh>();

    // Setup buffers
    if (primitive.attributes.HasPositions()) {
      gpu_mesh->position_buffer.set_storage(
          sizeof(glm::vec3) * primitive.attributes.positions.size(),
          primitive.attributes.positions.data(), GL_DYNAMIC_STORAGE_BIT);
    }

    if (primitive.attributes.HasNormals()) {
      gpu_mesh->normal_buffer.set_storage(
          sizeof(glm::vec3) * primitive.attributes.normals.size(),
          primitive.attributes.normals.data(), GL_DYNAMIC_STORAGE_BIT);
    }

    if (primitive.attributes.HasTexCoords0()) {
      gpu_mesh->texcoord_buffer.set_storage(
          sizeof(glm::vec2) * primitive.attributes.texcoords_0.size(),
          primitive.attributes.texcoords_0.data(), GL_DYNAMIC_STORAGE_BIT);
    }

    if (primitive.HasIndices()) {
      gpu_mesh->index_buffer.set_storage(
          sizeof(uint32_t) * primitive.indices.size(),
          primitive.indices.data(), GL_DYNAMIC_STORAGE_BIT);
      gpu_mesh->index_count = primitive.indices.size();
    }

    // Setup VAO
    gpu_mesh->vao.bind();

    // Position attribute (location 0)
    if (primitive.attributes.HasPositions()) {
      auto& binding_pos = gpu_mesh->vao.get_binding(0);
      binding_pos.bind_buffer(gpu_mesh->position_buffer, 0, sizeof(glm::vec3));
      auto& attr_pos = gpu_mesh->vao.get_attribute(0);
      attr_pos.set_format(3, GL_FLOAT, GL_FALSE, 0);
      attr_pos.bind(binding_pos);
      attr_pos.enable();
    }

    // Normal attribute (location 1)
    if (primitive.attributes.HasNormals()) {
      auto& binding_normal = gpu_mesh->vao.get_binding(1);
      binding_normal.bind_buffer(gpu_mesh->normal_buffer, 0, sizeof(glm::vec3));
      auto& attr_normal = gpu_mesh->vao.get_attribute(1);
      attr_normal.set_format(3, GL_FLOAT, GL_FALSE, 0);
      attr_normal.bind(binding_normal);
      attr_normal.enable();
    }

    // Texcoord attribute (location 2)
    if (primitive.attributes.HasTexCoords0()) {
      auto& binding_texcoord = gpu_mesh->vao.get_binding(2);
      binding_texcoord.bind_buffer(gpu_mesh->texcoord_buffer, 0, sizeof(glm::vec2));
      auto& attr_texcoord = gpu_mesh->vao.get_attribute(2);
      attr_texcoord.set_format(2, GL_FLOAT, GL_FALSE, 0);
      attr_texcoord.bind(binding_texcoord);
      attr_texcoord.enable();
    }

    // Element buffer
    if (primitive.HasIndices()) {
      gpu_mesh->vao.set_element_buffer(gpu_mesh->index_buffer);
    }

    gpu_meshes_[next_mesh_handle_++] = std::move(gpu_mesh);
  }
}

void ResourceManager::ProcessMaterial(const std::shared_ptr<sg::Material>& material) {
  auto gpu_material = std::make_unique<GPUMaterial>();

  // TODO: Extract material properties
  // For now, just create default material

  gpu_materials_[next_material_handle_++] = std::move(gpu_material);
}

void ResourceManager::ProcessTexture(const std::shared_ptr<sg::Texture>& texture) {
  if (texture->image) {
    gpu_textures_[next_texture_handle_++] = CreateTextureFromImage(texture->image);
  }
}

RenderSystem::RenderSystem() = default;

void RenderSystem::Initialize(std::shared_ptr<ResourceManager> resource_manager) {
  resource_manager_ = resource_manager;
  SetupShaders();

  // Setup sampler
  sampler_.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  sampler_.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  sampler_.set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  sampler_.set(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderSystem::SetupShaders() {
  // Create simple vertex shader
  const char* vertex_source = R"(
    #version 450 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aTexCoord;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 vNormal;
    out vec2 vTexCoord;

    void main() {
      gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
      vNormal = aNormal;
      vTexCoord = aTexCoord;
    }
  )";

  // Create simple fragment shader
  const char* fragment_source = R"(
    #version 450 core

    in vec3 vNormal;
    in vec2 vTexCoord;

    uniform sampler2D uBaseColorTexture;
    uniform vec4 uBaseColorFactor;

    out vec4 FragColor;

    void main() {
      vec4 baseColor = texture(uBaseColorTexture, vTexCoord) * uBaseColorFactor;
      FragColor = baseColor;
    }
  )";

  Shader vertex_shader(GL_VERTEX_SHADER);
  Shader fragment_shader(GL_FRAGMENT_SHADER);

  if (!vertex_shader.compile(vertex_source)) {
    LOG_ERROR("Vertex shader compilation failed: {}", vertex_shader.get_info_log());
    return;
  }

  if (!fragment_shader.compile(fragment_source)) {
    LOG_ERROR("Fragment shader compilation failed: {}", fragment_shader.get_info_log());
    return;
  }

  program_.attach(vertex_shader);
  program_.attach(fragment_shader);
  if (!program_.link()) {
    LOG_ERROR("Program linking failed: {}", program_.get_info_log());
    return;
  }

  // Get uniform locations
  model_loc_ = glGetUniformLocation(program_.get_name(), "uModel");
  view_loc_ = glGetUniformLocation(program_.get_name(), "uView");
  projection_loc_ = glGetUniformLocation(program_.get_name(), "uProjection");
  base_color_texture_loc_ = glGetUniformLocation(program_.get_name(), "uBaseColorTexture");
  base_color_factor_loc_ = glGetUniformLocation(program_.get_name(), "uBaseColorFactor");
}

void RenderSystem::Render(ecs::World& world) {
  if (!resource_manager_) {
    LOG_ERROR("RenderSystem not initialized with ResourceManager");
    return;
  }

  // Find primary camera
  glm::mat4 view_matrix = glm::mat4(1.0f);
  glm::mat4 projection_matrix = glm::mat4(1.0f);
  bool found_camera = false;

  auto camera_view = world.view<ecs::CameraComponent, ecs::TransformComponent>();
  for (auto entity : camera_view) {
    const auto& camera_comp = camera_view.get<ecs::CameraComponent>(entity);
    if (camera_comp.isPrimary) {
      const auto& transform_comp = camera_view.get<ecs::TransformComponent>(entity);

      // Calculate view matrix
      glm::vec3 position = transform_comp.position;
      glm::quat rotation = transform_comp.rotation;
      glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
      glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
      view_matrix = glm::lookAt(position, position + forward, up);

      // Calculate projection matrix (assuming 16:9 aspect ratio for now)
      projection_matrix = camera_comp.getProjectionMatrix(16.0f / 9.0f);

      found_camera = true;
      break;
    }
  }

  if (!found_camera) {
    LOG_WARN("No primary camera found, using identity matrices");
  }

  program_.use();

  // Set view and projection matrices
  glUniformMatrix4fv(view_loc_, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniformMatrix4fv(projection_loc_, 1, GL_FALSE, glm::value_ptr(projection_matrix));

  // Render all entities with MeshComponent and TransformComponent
  auto view = world.view<ecs::MeshComponent, ecs::TransformComponent>();
  for (auto entity : view) {
    const auto& mesh_comp = view.get<ecs::MeshComponent>(entity);
    const auto& transform_comp = view.get<ecs::TransformComponent>(entity);

    const GPUMesh* gpu_mesh = resource_manager_->GetGPUMesh(mesh_comp.meshHandle);
    const GPUMaterial* gpu_material = resource_manager_->GetGPUMaterial(mesh_comp.materialHandle);

    if (gpu_mesh && gpu_material) {
      glm::mat4 model_matrix = transform_comp.getTransformMatrix();
      RenderMesh(*gpu_mesh, *gpu_material, model_matrix);
    }
  }
}

void RenderSystem::RenderMesh(const GPUMesh& gpu_mesh, const GPUMaterial& gpu_material, const glm::mat4& model_matrix) {
  // Set model matrix
  glUniformMatrix4fv(model_loc_, 1, GL_FALSE, glm::value_ptr(model_matrix));

  // Bind base color texture
  if (gpu_material.base_color_texture) {
    gpu_material.base_color_texture->bind(0);
    sampler_.bind(0);
    glUniform1i(base_color_texture_loc_, 0);
  }

  // Set base color factor
  glUniform4fv(base_color_factor_loc_, 1, glm::value_ptr(gpu_material.base_color_factor));

  // Bind VAO and draw
  gpu_mesh.vao.bind();
  if (gpu_mesh.index_count > 0) {
    glDrawElements(gpu_mesh.mode, gpu_mesh.index_count, GL_UNSIGNED_INT, nullptr);
  } else {
    // TODO: Handle non-indexed meshes
  }
}

} // namespace rendering
} // namespace paimon