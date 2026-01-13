#include "paimon/core/io/gltf.h"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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
PrimitiveTopology parsePrimitiveMode(int primitiveMode) {
  switch (primitiveMode) {
  case TINYGLTF_MODE_POINTS:
    return PrimitiveTopology::Points;
  case TINYGLTF_MODE_LINE:
    return PrimitiveTopology::Lines;
  case TINYGLTF_MODE_LINE_LOOP:
    return PrimitiveTopology::LineLoop;
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

DataType parseCompnentType(int componentType) {
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

TextureFilterMode parseFilterMode(int filter) {
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

TextureWrapMode parseWrapMode(int wrap) {
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

  tinygltf::TinyGLTF loader;
  std::string errorMessage;
  std::string warningMessage;

  bool result = false;
  // Determine file type by extension
  if (filepath.extension() == ".glb") {
    result = loader.LoadBinaryFromFile(&m_model, &errorMessage,
                                       &warningMessage, filepath.string());
  } else {
    result = loader.LoadASCIIFromFile(&m_model, &errorMessage,
                                      &warningMessage, filepath.string());
  }

  if (!result) {
    LOG_WARN("Warning while loading glTF file: {}", warningMessage);
    LOG_ERROR("Failed to load glTF file: {}", errorMessage);
    return;
  }

}

void GltfLoader::load(ecs::Scene &scene) {
  parseBuffers();      // Step 1: buffer -> memory
  parseBufferViews();  // Step 2: bufferView -> memory (access buffer memory)
  parseAccessors();    // Step 3: accessor -> OpenGL Buffer (access bufferView memory)
  parseTextures();
  parseMaterials();
  parseMeshes();       // Step 4: mesh primitives reference accessor Buffers
  parseLights();
  parseCameras();

  int sceneIndex = m_model.defaultScene >= 0 ? m_model.defaultScene : 0;
  parseScene(m_model.scenes[sceneIndex], scene);
}

// ============================================================================
// Processing functions
// ============================================================================

void GltfLoader::parseBuffers() {
  LOG_INFO("Parsing {} buffers", m_model.buffers.size());
  
  for (const auto &buffer : m_model.buffers) {
    std::vector<uint8_t> bufferData(buffer.data.begin(), buffer.data.end());    
    m_buffers.push_back(std::move(bufferData));
  }
}

void GltfLoader::parseBufferViews() {
  LOG_INFO("Parsing {} buffer views", m_model.bufferViews.size());

  for (const auto &bufferView : m_model.bufferViews) {
    const auto &buffer = m_buffers[bufferView.buffer];
    auto begin = buffer.begin() + bufferView.byteOffset;
    std::vector<uint8_t> bufferViewData(begin, begin + bufferView.byteLength);
    m_bufferViews.push_back(std::move(bufferViewData));
  }
}

void GltfLoader::parseAccessors() {
  LOG_INFO("Parsing {} accessors", m_model.accessors.size());
  
  for (const auto &accessor : m_model.accessors) {   
    const auto &sourceBufferView = m_bufferViews[accessor.bufferView];
    const auto &bufferView = m_model.bufferViews[accessor.bufferView];
    
    // Use tinygltf's ByteStride utility function
    auto elementSize =
        tinygltf::GetComponentSizeInBytes(accessor.componentType) *
        tinygltf::GetNumComponentsInType(accessor.type);

    // Extract tightly packed data from bufferView
    std::vector<uint8_t> accessorData;
    accessorData.reserve(accessor.count * elementSize);
    
    auto stride = accessor.ByteStride(bufferView);
    for (size_t j = 0; j < accessor.count; ++j) {
      auto offset = accessor.byteOffset + j * stride;
      auto begin = sourceBufferView.begin() + offset;
      accessorData.insert(accessorData.end(), begin, begin + elementSize);
    }
    
    // Create OpenGL Buffer with tightly packed data
    auto gl_buffer = std::make_shared<Buffer>();
    gl_buffer->set_storage(accessorData.size(), accessorData.data(), 0);    
    m_accessors.push_back(std::move(gl_buffer));
  }
}

void GltfLoader::parseTextures() {
  for (const auto &texture : m_model.textures) {
    auto sg_texture = std::make_shared<sg::Texture>();
    sg_texture->image = parse(m_model.images[texture.source]);
    
    // Check if sampler is valid, use default if not
    if (texture.sampler >= 0) {
      sg_texture->sampler = parse(m_model.samplers[texture.sampler]);
    } else {
      sg_texture->sampler = getDefaultSampler();
    }
    
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
            material.pbrMetallicRoughness.baseColorTexture.index >= 0
                ? m_textures[material.pbrMetallicRoughness.baseColorTexture.index]
                : nullptr,
        .metallicFactor =
            static_cast<float>(material.pbrMetallicRoughness.metallicFactor),
        .roughnessFactor =
            static_cast<float>(material.pbrMetallicRoughness.roughnessFactor),
        .metallicRoughnessTexture =
            material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0
                ? m_textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index]
                : nullptr,
    };

    sg_material->normalTexture = material.normalTexture.index >= 0
                                     ? m_textures[material.normalTexture.index]
                                     : nullptr;
    sg_material->normalScale = material.normalTexture.scale;

    sg_material->occlusionTexture =
        material.occlusionTexture.index >= 0
            ? m_textures[material.occlusionTexture.index]
            : nullptr;
    sg_material->occlusionStrength = material.occlusionTexture.strength;

    sg_material->emissiveTexture =
        material.emissiveTexture.index >= 0
            ? m_textures[material.emissiveTexture.index]
            : nullptr;
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
      sg_primitive.mode = parsePrimitiveMode(primitive.mode);

      // First handle attributes (vertex data)
      for (const auto &[attributeName, accessorIndex] : primitive.attributes) {
        const auto &accessor = m_model.accessors[accessorIndex];

        sg_primitive.vertexCount = accessor.count;

        // Assign the OpenGL buffer from accessor (independent buffer layout)
        if (attributeName == "POSITION") {
          sg_primitive.positions = m_accessors[accessorIndex];
        } else if (attributeName == "NORMAL") {
          sg_primitive.normals = m_accessors[accessorIndex];
        } else if (attributeName.rfind("TEXCOORD", 0) == 0) {
          // assign first texcoord into texcoords (TEXCOORD_0)
          if (attributeName == "TEXCOORD_0") {
            sg_primitive.texcoords = m_accessors[accessorIndex];
          }
        } else if (attributeName.rfind("COLOR", 0) == 0) {
          sg_primitive.colors = m_accessors[accessorIndex];
        } else {
          // other attributes can be ignored for now or handled later
        }
      }

      // Then handle indices (element buffer) if present
      if (primitive.indices >= 0) {
        const auto &accessor = m_model.accessors[primitive.indices];
        sg_primitive.indexCount = accessor.count;
        sg_primitive.indexType = parseCompnentType(accessor.componentType);
        sg_primitive.indices = m_accessors[primitive.indices];
      }
    }
    m_meshes.push_back(std::move(sg_mesh));
  }
}

void GltfLoader::parseLights() {
  for (const auto &light : m_model.lights) {
   std::shared_ptr<sg::PunctualLight> sg_light;

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

void GltfLoader::parseCameras() {
  for (const auto &camera : m_model.cameras) {
    std::shared_ptr<sg::Camera> sg_camera;

    if (camera.type == "perspective") {
      auto perspective = std::make_shared<sg::PerspectiveCamera>();
      perspective->yfov = static_cast<float>(camera.perspective.yfov);
      perspective->znear = static_cast<float>(camera.perspective.znear);
      perspective->zfar = static_cast<float>(camera.perspective.zfar);
      if (camera.perspective.aspectRatio > 0.0) {
        perspective->aspectRatio =
            static_cast<float>(camera.perspective.aspectRatio);
      }
      sg_camera = perspective;
    } else if (camera.type == "orthographic") {
      auto orthographic = std::make_shared<sg::OrthographicCamera>();
      orthographic->xmag = static_cast<float>(camera.orthographic.xmag);
      orthographic->ymag = static_cast<float>(camera.orthographic.ymag);
      orthographic->znear = static_cast<float>(camera.orthographic.znear);
      orthographic->zfar = static_cast<float>(camera.orthographic.zfar);
      sg_camera = orthographic;
    } else {
      LOG_WARN("Unsupported camera type: {}", camera.type);
      continue;
    }

    m_cameras.push_back(std::move(sg_camera));
  }
}

void GltfLoader::parseNode(const tinygltf::Node &node, ecs::Entity parent, ecs::Scene &scene) {
  // Create entity for this node
  auto nodeEntity = scene.createEntity(node.name);

  // Parent components
  auto &parentComp = nodeEntity.getComponent<ecs::Parent>();
  parentComp.parent = parent;

  // Parent should have a Children component; append this child
  auto &parentChildren = parent.getComponent<ecs::Children>();
  parentChildren.children.push_back(nodeEntity);

  // Transform component (TRS only)
  auto &transform = nodeEntity.getComponent<ecs::Transform>();
  if (node.matrix.empty()) {
    // Use TRS directly
    transform.translation = node.translation.empty()
                                ? glm::vec3(0.0f)
                                : parseVec3(node.translation);
    transform.rotation = node.rotation.empty()
                             ? glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
                             : parseQuat(node.rotation);
    transform.scale = node.scale.empty() ? glm::vec3(1.0f)
                                         : parseVec3(node.scale);
  } else {
    // Decompose matrix into TRS
    auto matrix = parseMat4(node.matrix);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, transform.scale, transform.rotation,
                   transform.translation, skew, perspective);
  }

  // Mesh Component - create child entities for each primitive
  if (node.mesh >= 0) {
    const auto& sg_mesh = m_meshes[node.mesh];
    const auto& mesh = m_model.meshes[node.mesh];
    
    for (size_t i = 0; i < sg_mesh->primitives.size(); ++i) {
      // Create child entity for each primitive
      auto primitiveEntity = scene.createEntity(node.name + "_primitive_" + std::to_string(i));
      
      // Set parent relationship
      auto& primitiveParentComp = primitiveEntity.getComponent<ecs::Parent>();
      primitiveParentComp.parent = nodeEntity;
      
      // Add to parent's children
      auto& entityChildren = nodeEntity.getComponent<ecs::Children>();
      entityChildren.children.push_back(primitiveEntity);
      
      // Add Primitive component
      auto primitive = std::make_shared<sg::Primitive>(sg_mesh->primitives[i]);
      primitiveEntity.addComponent<ecs::Primitive>(primitive);

      // Add Material component if available
      // Note: In glTF, a mesh primitive can reference a material
      if (mesh.primitives[i].material >= 0) {
        primitiveEntity.addComponent<ecs::Material>(m_materials[mesh.primitives[i].material]);
      }
    }
  }

  // Light Component (from KHR_lights_punctual extension)
  if (node.light >= 0) {
    nodeEntity.addComponent<ecs::PunctualLight>(m_lights[node.light]);
  }

  if (node.camera >= 0) {
    // Camera Component can be handled here if needed
    nodeEntity.addComponent<ecs::Camera>(m_cameras[node.camera]);
  }

  // Skin Component
  if (node.skin >= 0) {
    // Skins can be handled here if needed
  }

  // Recursively parse child nodes (depth-first order)
  // This ensures parent entities are created before their children,
  // which simplifies GlobalTransform computation later
  for (int childIndex : node.children) {
    const auto &childNode = m_model.nodes[childIndex];
    parseNode(childNode, nodeEntity, scene);
  }
}

void GltfLoader::parseScene(const tinygltf::Scene &scene, ecs::Scene &ecs_scene) {
  // Implementation for parsing a glTF scene into an ECS scene
  m_rootEntity = ecs_scene.createEntity("RootNode");
  for (const auto nodeIndex : scene.nodes) {
    const auto &node = m_model.nodes[nodeIndex];
    parseNode(node, m_rootEntity, ecs_scene);
  }
}
