#include "paimon/core/io/gltf.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "glm/fwd.hpp"
#include "paimon/core/ecs/components.h"
#include "paimon/core/ecs/entity.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/texture.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/type.h"

using namespace paimon;

namespace {
PrimitiveTopology castPrimitiveMode(int primitiveMode) {
  switch (primitiveMode) {
  case TINYGLTF_MODE_POINTS:
    return PrimitiveTopology::Points;
  case TINYGLTF_MODE_LINE:
    return PrimitiveTopology::Lines;
  case TINYGLTF_MODE_LINE_LOOP:
    return PrimitiveTopology::LinesLoop;
  case TINYGLTF_MODE_LINE_STRIP:
    return PrimitiveTopology::LineStrip;
  case TINYGLTF_MODE_TRIANGLES:
    return PrimitiveTopology::Triangles;
  case TINYGLTF_MODE_TRIANGLE_STRIP:
    return PrimitiveTopology::TriangleStrip;
  case TINYGLTF_MODE_TRIANGLE_FAN:
    return PrimitiveTopology::TriangleFan;
  default:
    return PrimitiveTopology::Triangles;
  }
}

DataType castCompnentType(int componentType) {
  switch (componentType) {
  case TINYGLTF_COMPONENT_TYPE_BYTE:
    return DataType::Byte;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
    return DataType::UByte;
  case TINYGLTF_COMPONENT_TYPE_SHORT:
    return DataType::Short;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
    return DataType::UShort;
  case TINYGLTF_COMPONENT_TYPE_INT:
    return DataType::Int;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
    return DataType::UInt;
  case TINYGLTF_COMPONENT_TYPE_FLOAT:
    return DataType::Float;
  case TINYGLTF_COMPONENT_TYPE_DOUBLE:
    return DataType::Double;
  default:
    return DataType::Float;
  }
}

TextureFilterMode castFilterMode(int filter) {
  switch (filter) {
  case TINYGLTF_TEXTURE_FILTER_NEAREST:
    return TextureFilterMode::Nearest;
  case TINYGLTF_TEXTURE_FILTER_LINEAR:
    return TextureFilterMode::Linear;
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
    return TextureFilterMode::NearestMipmapNearest;
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
    return TextureFilterMode::LinearMipmapNearest;
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
    return TextureFilterMode::NearestMipmapLinear;
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    return TextureFilterMode::LinearMipmapLinear;
  default:
    return TextureFilterMode::Linear;
  }
}

TextureWrapMode castWrapMode(int wrap) {
  switch (wrap) {
  case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
    return TextureWrapMode::ClampToEdge;
  case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
    return TextureWrapMode::MirroredRepeat;
  case TINYGLTF_TEXTURE_WRAP_REPEAT:
    return TextureWrapMode::Repeat;
  default:
    return TextureWrapMode::Repeat;
  }
}

std::shared_ptr<Sampler> parse(const tinygltf::Sampler& sampler) {
  auto gl_sampler = std::make_shared<Sampler>();

  // Set min filter
  if (sampler.minFilter >= 0) {
    gl_sampler->set<GLenum>(GL_TEXTURE_MIN_FILTER,
                            static_cast<GLenum>(sampler.minFilter));
  } else {
    gl_sampler->set<GLenum>(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }

  // Set mag filter
  if (sampler.magFilter >= 0) {
    gl_sampler->set<GLenum>(GL_TEXTURE_MAG_FILTER,
                            static_cast<GLenum>(sampler.magFilter));
  } else {
    gl_sampler->set<GLenum>(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // Set wrap modes
  gl_sampler->set<GLenum>(GL_TEXTURE_WRAP_S,
                          static_cast<GLenum>(sampler.wrapS));
  gl_sampler->set<GLenum>(GL_TEXTURE_WRAP_T,
                          static_cast<GLenum>(sampler.wrapT));

  return gl_sampler;
}

std::shared_ptr<Sampler> getDefaultSampler() {
  auto gl_sampler = std::make_shared<Sampler>();
  // Default sampler settings
  gl_sampler->set<GLenum>(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  gl_sampler->set<GLenum>(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gl_sampler->set<GLenum>(GL_TEXTURE_WRAP_S, GL_REPEAT);
  gl_sampler->set<GLenum>(GL_TEXTURE_WRAP_T, GL_REPEAT);
  return gl_sampler;
}

std::shared_ptr<Texture> parse(const tinygltf::Image& image) {
  auto gl_texture = std::make_shared<Texture>(GL_TEXTURE_2D);

  // Determine format
  GLenum internalFormat = GL_RGBA8;
  GLenum format = GL_RGBA;

  if (image.component == 1) {
    internalFormat = GL_R8;
    format = GL_RED;
  } else if (image.component == 2) {
    internalFormat = GL_RG8;
    format = GL_RG;
  } else if (image.component == 3) {
    internalFormat = GL_RGB8;
    format = GL_RGB;
  }

  // Upload to GPU
  gl_texture->set_storage_2d(1, internalFormat, image.width, image.height);
  gl_texture->set_sub_image_2d(0, 0, 0, image.width, image.height,
                               format, GL_UNSIGNED_BYTE, image.image.data());
  gl_texture->generate_mipmap();

  return gl_texture;
}

glm::vec4 parseVec4(const std::vector<double> data) {
  return glm::vec4(static_cast<float>(data[0]),
                   static_cast<float>(data[1]),
                   static_cast<float>(data[2]),
                   static_cast<float>(data[3]));
}

glm::vec3 parseVec3(const std::vector<double> data) {
  return glm::vec3(static_cast<float>(data[0]),
                   static_cast<float>(data[1]),
                   static_cast<float>(data[2]));
}

sg::AlphaMode parseAlphaMode(const std::string &modeStr) {
  if (modeStr == "OPAQUE") {
    return sg::AlphaMode::Opaque;
  } else if (modeStr == "MASK") {
    return sg::AlphaMode::Mask;
  } else if (modeStr == "BLEND") {
    return sg::AlphaMode::Blend;
  } else {
    return sg::AlphaMode::Opaque;
  }
}

} // namespace

// ============================================================================
// GltfLoader methods
// ============================================================================

GltfLoader::GltfLoader(const std::filesystem::path &filepath) {

  std::string errorMessage;
  std::string warningMessage;

  bool result = false;
  // Determine file type by extension
  if (filepath.extension() == ".glb") {
    result = m_loader.LoadBinaryFromFile(&m_model, &errorMessage,
                                       &warningMessage, filepath);
  } else {
    result = m_loader.LoadASCIIFromFile(&m_model, &errorMessage,
                                      &warningMessage, filepath);
  }

  if (!result) {
    LOG_WARN("Warning while loading glTF file: {}", warningMessage);
    LOG_ERROR("Failed to load glTF file: {}", errorMessage);
    return;
  }

  processTextures(m_model);
  processMaterials(m_model);
  processMeshes(m_model);
}

void GltfLoader::loadScene(ecs::Scene &scene, int scene_index) {
  // Assume model is already loaded in constructor

  processNodes(m_model, scene);
  processScenes(m_model, scene, scene_index);
}

// ============================================================================
// Processing functions
// ============================================================================

void GltfLoader::parseTexture(const tinygltf::Texture &texture) {
  auto sg_texture = std::make_shared<sg::Texture>();

  sg_texture->image = parse(m_model.images[texture.source]);

  sg_texture->sampler = parse(m_model.samplers[texture.sampler]);

  m_textures.push_back(std::move(sg_texture));
}

void GltfLoader::pasrseMaterial(const tinygltf::Material &material) {
  auto sg_material = std::make_shared<sg::Material>();

  sg_material->pbrMetallicRoughness = {
    .baseColorFactor = parseVec4(material.pbrMetallicRoughness.baseColorFactor),
    .baseColorTexture = m_textures[material.pbrMetallicRoughness.baseColorTexture.index],
    .metallicFactor = static_cast<float>(material.pbrMetallicRoughness.metallicFactor),
    .roughnessFactor = static_cast<float>(material.pbrMetallicRoughness.roughnessFactor),
    .metallicRoughnessTexture = m_textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index],
  };

  sg_material->normalTexture = m_textures[material.normalTexture.index];
  sg_material->normalScale = material.normalTexture.scale;

  sg_material->occlusionTexture = m_textures[material.occlusionTexture.index];
  sg_material->occlusionStrength = material.occlusionTexture.strength;

  sg_material->emissiveTexture = m_textures[material.emissiveTexture.index];
  sg_material->emissiveFactor = parseVec3(material.emissiveFactor);

  sg_material->doubleSided = material.doubleSided;
  sg_material->alphaMode = parseAlphaMode(material.alphaMode);
  sg_material->alphaCutoff = static_cast<float>(material.alphaCutoff);

  m_materials.push_back(std::move(sg_material));
}

void GltfLoader::parseMesh(const tinygltf::Mesh &mesh) {
  auto sg_mesh = std::make_shared<sg::Mesh>();

  for (const auto &primitive : mesh.primitives) {
    auto &sg_primitive = sg_mesh->primitives.emplace_back();

    // Primitive topology
    sg_primitive.mode = castPrimitiveMode(primitive.mode);

    for (const auto &[attributeName, accessorIndex] : primitive.attributes) {
      const auto &accessor = m_model.accessors[accessorIndex];
      const auto &bufferView = m_model.bufferViews[accessor.bufferView];
      const auto &buffer = m_model.buffers[bufferView.buffer];

      auto gl_buffer = std::make_shared<Buffer>();
      gl_buffer->set_storage(bufferView.byteLength, 
                             buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);

      m_buffers.push_back(std::move(gl_buffer));
    }

    sg_primitive.material = m_materials[primitive.material];
  }
  m_meshes.push_back(std::move(sg_mesh));
}

void GltfLoader::parseLight(const tinygltf::Light &light) {
  std::shared_ptr<sg::PunctualLight> sg_light = nullptr;

  if (light.type == "directional") {
    sg_light = std::make_shared<sg::DirectionalLight>();
  } else if (light.type == "point") {
    sg_light = std::make_shared<sg::PointLight>();
  } else if (light.type == "spot") {
    sg_light = std::make_shared<sg::SpotLight>();
  } else {
    LOG_WARN("Unsupported light type: {}", light.type);
    return;
  }

  sg_light->color = parseVec3(light.color);
  sg_light->intensity = static_cast<float>(light.intensity);
  sg_light->range = static_cast<float>(light.range);

  if (light.type == "spot") {
    auto spot_light = std::dynamic_pointer_cast<sg::SpotLight>(sg_light);
    spot_light->innerConeAngle =
        static_cast<float>(light.spot.innerConeAngle);
    spot_light->outerConeAngle =
        static_cast<float>(light.spot.outerConeAngle);
  }

  m_lights.push_back(std::move(sg_light));
}

void GltfLoader::processNodes(const tinygltf::Model &model, ecs::Scene &scene) {
  for (size_t i = 0; i < model.nodes.size(); ++i) {

  }
}

void GltfLoader::processScenes(const tinygltf::Model &model, ecs::Scene &scene) {
  // Use default scene or first scene
  int scene_index = model.defaultScene >= 0 ? model.defaultScene : 0;

  if (scene_index >= 0 &&
      static_cast<size_t>(scene_index) < model.scenes.size()) {
    const auto &gltf_scene = model.scenes[scene_index];
    
    // Root nodes in glTF are those listed in the scene
    // Their hierarchy components should already have parent = entt::null
    // which indicates they are root nodes
    
    // Optional: You could tag root nodes with a special component if needed
    for (int node_index : gltf_scene.nodes) {
      if (m_node2entity.find(node_index) != m_node2entity.end()) {
        // The node entity is already created and configured
        // Just verify it's a root node (parent should be null)
        auto entity_handle = m_node2entity[node_index];
        auto &hierarchy = scene.getRegistry().get<ecs::Hierarchy>(entity_handle);
        // Root nodes should have parent = entt::null (which is the default)
      }
    }
  }
}
