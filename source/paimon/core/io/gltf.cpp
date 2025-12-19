#include "paimon/core/io/gltf.h"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "glm/fwd.hpp"
#include "paimon/core/ecs/components.h"
#include "paimon/core/ecs/entity.h"
#include "paimon/core/log_system.h"
#include "paimon/core/sg/mesh.h"
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
  return glm::vec4(data[0], data[1], data[2], data[3]);
}

glm::vec3 parseVec3(const std::vector<double> data) {
  return glm::vec3(data[0], data[1], data[2]);
}

glm::quat parseQuat(const std::vector<double> data) {
  return glm::quat(static_cast<float>(data[3]), static_cast<float>(data[0]),
                   static_cast<float>(data[1]), static_cast<float>(data[2]));
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

glm::mat4 parseMat4(const std::vector<double> data) {
  glm::mat4 mat{data[0],  data[1],  data[2],  data[3], data[4],  data[5],
                data[6],  data[7],  data[8],  data[9], data[10], data[11],
                data[12], data[13], data[14], data[15]};
  return mat;
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

}

void GltfLoader::load(ecs::Scene &scene) {
  parseBuffers();
  parseBufferViews();
  parseTextures();
  parseMaterials();
  parseScene(m_model.scenes[m_model.defaultScene], scene);
}

void GltfLoader::load(ecs::Scene &scene, int scene_index) {
  parseBuffers();
  parseBufferViews();
  parseTextures();
  parseMaterials();
  parseScene(m_model.scenes[scene_index], scene);
}

// ============================================================================
// Processing functions
// ============================================================================

void GltfLoader::parseBuffers() {
  for (const auto &buffer : m_model.buffers) {
    auto gl_buffer = std::make_shared<Buffer>();
    gl_buffer->set_storage(buffer.data.size(), buffer.data.data());
    m_buffers.push_back(std::move(gl_buffer));
  }
}

void GltfLoader::parseBufferViews() {
  for (const auto &bufferView : m_model.bufferViews) {
    auto gl_buffer = std::make_shared<Buffer>();
    gl_buffer->set_storage(bufferView.byteLength, nullptr);
    gl_buffer->copy_sub_data(*m_buffers[bufferView.buffer], bufferView.byteOffset, 0, bufferView.byteLength);
    m_bufferViews.push_back(std::move(gl_buffer));
  }
}

void GltfLoader::parseTextures() {
  for (const auto &texture : m_model.textures) {
    auto sg_texture = std::make_shared<sg::Texture>();
    sg_texture->image = parse(m_model.images[texture.source]);
    sg_texture->sampler = parse(m_model.samplers[texture.sampler]);
    m_textures.push_back(std::move(sg_texture));
  }
}

void GltfLoader::parseMaterials() {
  for (const auto &material : m_model.materials) {
    auto sg_material = std::make_shared<sg::Material>();
    sg_material->pbrMetallicRoughness = {
        .baseColorFactor =
            parseVec4(material.pbrMetallicRoughness.baseColorFactor),
        .baseColorTexture =
            m_textures[material.pbrMetallicRoughness.baseColorTexture.index],
        .metallicFactor =
            static_cast<float>(material.pbrMetallicRoughness.metallicFactor),
        .roughnessFactor =
            static_cast<float>(material.pbrMetallicRoughness.roughnessFactor),
        .metallicRoughnessTexture =
            m_textures[material.pbrMetallicRoughness.metallicRoughnessTexture
                           .index],
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
}

void GltfLoader::parseMeshes() {
  for (const auto &mesh : m_model.meshes) {
    auto sg_mesh = std::make_shared<sg::Mesh>();

    for (const auto &primitive : mesh.primitives) {
      auto &sg_primitive = sg_mesh->primitives.emplace_back();
      // Primitive topology
      sg_primitive.mode = castPrimitiveMode(primitive.mode);

      // First handle attributes (vertex data)
      for (const auto &[attributeName, accessorIndex] : primitive.attributes) {
        const auto &accessor = m_model.accessors[accessorIndex];
        const auto &bufferView = m_bufferViews[accessor.bufferView];

        // Assign the GL buffer to the corresponding primitive attribute
        if (attributeName == "POSITION") {
          sg_primitive.positions = bufferView;
        } else if (attributeName == "NORMAL") {
          sg_primitive.normals = bufferView;
        } else if (attributeName.rfind("TEXCOORD", 0) == 0) {
          // assign first texcoord into texcoords (TEXCOORD_0)
          if (attributeName == "TEXCOORD_0") {
            sg_primitive.texcoords = bufferView;
          }
        } else if (attributeName.rfind("COLOR", 0) == 0) {
          sg_primitive.colors = bufferView;
        } else {
          // other attributes can be ignored for now or handled later
        }
      }

      // Then handle indices (element buffer) if present
      if (primitive.indices >= 0) {
        const auto &accessor = m_model.accessors[primitive.indices];
        sg_primitive.indices = m_bufferViews[accessor.bufferView];
      }

      sg_primitive.material = m_materials[primitive.material];
    }
    m_meshes.push_back(std::move(sg_mesh));
  }
}

void GltfLoader::parseLights() {
  for (const auto &light : m_model.lights) {
    auto sg_light = std::shared_ptr<sg::PunctualLight>();

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
}

void GltfLoader::parseNode(const tinygltf::Node &node, ecs::Entity parent, ecs::Scene &scene) {
  // Create entity for this node
  auto entity = scene.createEntity();

  // Name component
  entity.addComponent<ecs::Name>(node.name);

  // Hierarchy Component
  auto &hierarchy = entity.addComponent<ecs::Hierarchy>();
  hierarchy.parent = parent;
  auto &parentHierarchy = parent.getComponent<ecs::Hierarchy>();
  parentHierarchy.children.push_back(entity);

  // Transform component
  auto &transform = entity.addComponent<ecs::Transform>();
  if (node.matrix.empty()) {
    // Use TRS
    transform.translation = node.translation.empty()
                                ? glm::vec3(0.0f)
                                : parseVec3(node.translation);
    transform.rotation = node.rotation.empty()
                             ? glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
                             : parseQuat(node.rotation);
    transform.scale = node.scale.empty() ? glm::vec3(1.0f)
                                         : parseVec3(node.scale);

    transform.matrix =
        glm::translate(glm::mat4(1.0f), transform.translation) *
        glm::mat4_cast(transform.rotation) *
        glm::scale(glm::mat4(1.0f), transform.scale);
  } else {
    // Use matrix
    transform.matrix = parseMat4(node.matrix);

    // Decompose matrix into TRS
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform.matrix, transform.scale, transform.rotation,
                   transform.translation, skew, perspective);
  }

  // Mesh Component
  if (node.mesh >= 0) {
    entity.addComponent<ecs::Mesh>(m_meshes[node.mesh]);
  }

  // Light Component (from KHR_lights_punctual extension)
  if (node.light >= 0) {
    entity.addComponent<ecs::PunctualLight>(m_lights[node.light]);
  }

  // Skin Component
  if (node.skin >= 0) {
    // Skins can be handled here if needed
  }
}

void GltfLoader::parseScene(const tinygltf::Scene &scene, ecs::Scene &ecs_scene) {
  // Implementation for parsing a glTF scene into an ECS scene
  for (const auto nodeIndex : scene.nodes) {
    const auto &node = m_model.nodes[nodeIndex];

    auto rootEntity = ecs_scene.createEntity();
    parseNode(node, rootEntity, ecs_scene);
  }
}
