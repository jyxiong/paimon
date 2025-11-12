#include "paimon/core/io/gltf.h"

#include <tiny_gltf.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace paimon {

// ============================================================================
// Template specializations for accessor data extraction (forward declarations)
// ============================================================================

template <>
void GltfLoader::ExtractAccessorData<glm::vec3>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<glm::vec3> &out_data);

template <>
void GltfLoader::ExtractAccessorData<glm::vec2>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<glm::vec2> &out_data);

template <>
void GltfLoader::ExtractAccessorData<glm::vec4>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<glm::vec4> &out_data);

template <>
void GltfLoader::ExtractAccessorData<uint32_t>(const tinygltf::Model &model,
                                               int accessor_index,
                                               std::vector<uint32_t> &out_data);

// ============================================================================
// GltfLoader methods
// ============================================================================

bool GltfLoader::LoadFromFile(const std::string &filepath,
                              sg::Scene &out_scene) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;

  error_message_.clear();
  warning_message_.clear();

  bool result = false;

  // Determine file type by extension
  if (filepath.find(".glb") != std::string::npos) {
    result = loader.LoadBinaryFromFile(&model, &error_message_,
                                       &warning_message_, filepath);
  } else {
    result = loader.LoadASCIIFromFile(&model, &error_message_,
                                      &warning_message_, filepath);
  }

  if (!result) {
    return false;
  }

  // Process model data in order (images -> samplers -> textures -> materials ->
  // meshes -> nodes -> scenes)
  ProcessImages(model, out_scene);
  ProcessSamplers(model, out_scene);
  ProcessTextures(model, out_scene);
  ProcessMaterials(model, out_scene);
  ProcessMeshes(model, out_scene);

  // Temporary node storage for building hierarchy
  std::vector<std::shared_ptr<sg::Node>> temp_nodes;
  ProcessNodes(model, out_scene, temp_nodes);
  ProcessScenes(model, out_scene, temp_nodes);

  return true;
}

// ============================================================================
// Processing functions
// ============================================================================

void GltfLoader::ProcessImages(const tinygltf::Model &model, sg::Scene &scene) {
  scene.images.reserve(model.images.size());

  for (const auto &image : model.images) {
    auto sg_image = std::make_shared<sg::Image>();
    sg_image->name = image.name;
    sg_image->uri = image.uri;
    sg_image->mime_type = image.mimeType;
    sg_image->width = image.width;
    sg_image->height = image.height;
    sg_image->components = image.component;
    sg_image->bits = image.bits;
    sg_image->data = image.image;

    if (sg_image->data.empty() && !image.uri.empty()) {
      warning_message_ += "Warning: Image data is empty for image '" +
                          image.name + "' (URI: " + image.uri + ")\n";
    }

    scene.images.push_back(std::move(sg_image));
  }
}

void GltfLoader::ProcessSamplers(const tinygltf::Model &model,
                                 sg::Scene &scene) {
  scene.samplers.reserve(model.samplers.size());

  for (const auto &sampler : model.samplers) {
    auto sg_sampler = std::make_shared<sg::Sampler>();
    sg_sampler->name = sampler.name;

    // Min filter
    if (sampler.minFilter >= 0) {
      sg_sampler->min_filter =
          static_cast<sg::Sampler::MinFilter>(sampler.minFilter);
    }

    // Mag filter
    if (sampler.magFilter >= 0) {
      sg_sampler->mag_filter =
          static_cast<sg::Sampler::MagFilter>(sampler.magFilter);
    }

    // Wrap modes
    sg_sampler->wrap_s = static_cast<sg::Sampler::WrapMode>(sampler.wrapS);
    sg_sampler->wrap_t = static_cast<sg::Sampler::WrapMode>(sampler.wrapT);

    scene.samplers.push_back(std::move(sg_sampler));
  }
}

void GltfLoader::ProcessTextures(const tinygltf::Model &model,
                                 sg::Scene &scene) {
  scene.textures.reserve(model.textures.size());

  for (const auto &texture : model.textures) {
    auto sg_texture = std::make_shared<sg::Texture>();
    sg_texture->name = texture.name;

    // Link to image
    if (texture.source >= 0 &&
        static_cast<size_t>(texture.source) < scene.images.size()) {
      sg_texture->image = scene.images[texture.source];
    }

    // Link to sampler
    if (texture.sampler >= 0 &&
        static_cast<size_t>(texture.sampler) < scene.samplers.size()) {
      sg_texture->sampler = scene.samplers[texture.sampler];
    } else {
      // Create default sampler if not specified
      sg_texture->sampler = std::make_shared<sg::Sampler>();
    }

    scene.textures.push_back(std::move(sg_texture));
  }
}

void GltfLoader::ProcessMaterials(const tinygltf::Model &model,
                                  sg::Scene &scene) {
  scene.materials.reserve(model.materials.size());

  for (const auto &material : model.materials) {
    auto sg_material = std::make_shared<sg::Material>();
    sg_material->name = material.name;

    // PBR Metallic Roughness
    if (material.values.find("baseColorFactor") != material.values.end()) {
      const auto &factor = material.values.at("baseColorFactor");
      if (factor.number_array.size() == 4) {
        sg_material->pbr_metallic_roughness.base_color_factor =
            glm::vec4(static_cast<float>(factor.number_array[0]),
                      static_cast<float>(factor.number_array[1]),
                      static_cast<float>(factor.number_array[2]),
                      static_cast<float>(factor.number_array[3]));
      }
    }

    if (material.values.find("baseColorTexture") != material.values.end()) {
      int tex_index =
          material.values.at("baseColorTexture").json_double_value.at("index");
      if (tex_index >= 0 &&
          static_cast<size_t>(tex_index) < scene.textures.size()) {
        sg_material->pbr_metallic_roughness.base_color_texture =
            scene.textures[tex_index];
      }
    }

    if (material.values.find("metallicFactor") != material.values.end()) {
      sg_material->pbr_metallic_roughness.metallic_factor =
          static_cast<float>(material.values.at("metallicFactor").number_value);
    }

    if (material.values.find("roughnessFactor") != material.values.end()) {
      sg_material->pbr_metallic_roughness.roughness_factor = static_cast<float>(
          material.values.at("roughnessFactor").number_value);
    }

    if (material.values.find("metallicRoughnessTexture") !=
        material.values.end()) {
      int tex_index = material.values.at("metallicRoughnessTexture")
                          .json_double_value.at("index");
      if (tex_index >= 0 &&
          static_cast<size_t>(tex_index) < scene.textures.size()) {
        sg_material->pbr_metallic_roughness.metallic_roughness_texture =
            scene.textures[tex_index];
      }
    }

    // Normal texture
    if (material.additionalValues.find("normalTexture") !=
        material.additionalValues.end()) {
      int tex_index = material.additionalValues.at("normalTexture")
                          .json_double_value.at("index");
      if (tex_index >= 0 &&
          static_cast<size_t>(tex_index) < scene.textures.size()) {
        sg_material->normal_texture = scene.textures[tex_index];
      }
    }

    // Occlusion texture
    if (material.additionalValues.find("occlusionTexture") !=
        material.additionalValues.end()) {
      int tex_index = material.additionalValues.at("occlusionTexture")
                          .json_double_value.at("index");
      if (tex_index >= 0 &&
          static_cast<size_t>(tex_index) < scene.textures.size()) {
        sg_material->occlusion_texture = scene.textures[tex_index];
      }
    }

    // Emissive
    if (material.additionalValues.find("emissiveTexture") !=
        material.additionalValues.end()) {
      int tex_index = material.additionalValues.at("emissiveTexture")
                          .json_double_value.at("index");
      if (tex_index >= 0 &&
          static_cast<size_t>(tex_index) < scene.textures.size()) {
        sg_material->emissive_texture = scene.textures[tex_index];
      }
    }

    if (material.additionalValues.find("emissiveFactor") !=
        material.additionalValues.end()) {
      const auto &factor = material.additionalValues.at("emissiveFactor");
      if (factor.number_array.size() == 3) {
        sg_material->emissive_factor =
            glm::vec3(static_cast<float>(factor.number_array[0]),
                      static_cast<float>(factor.number_array[1]),
                      static_cast<float>(factor.number_array[2]));
      }
    }

    // Double sided
    if (material.additionalValues.find("doubleSided") !=
        material.additionalValues.end()) {
      sg_material->double_sided =
          material.additionalValues.at("doubleSided").bool_value;
    }

    // Alpha mode
    if (material.additionalValues.find("alphaMode") !=
        material.additionalValues.end()) {
      const std::string &alpha_mode =
          material.additionalValues.at("alphaMode").string_value;
      if (alpha_mode == "OPAQUE") {
        sg_material->alpha_mode = sg::Material::AlphaMode::Opaque;
      } else if (alpha_mode == "MASK") {
        sg_material->alpha_mode = sg::Material::AlphaMode::Mask;
      } else if (alpha_mode == "BLEND") {
        sg_material->alpha_mode = sg::Material::AlphaMode::Blend;
      }
    }

    if (material.additionalValues.find("alphaCutoff") !=
        material.additionalValues.end()) {
      sg_material->alpha_cutoff = static_cast<float>(
          material.additionalValues.at("alphaCutoff").number_value);
    }

    scene.materials.push_back(std::move(sg_material));
  }
}

void GltfLoader::ProcessMeshes(const tinygltf::Model &model, sg::Scene &scene) {
  scene.meshes.reserve(model.meshes.size());

  for (const auto &mesh : model.meshes) {
    auto sg_mesh = std::make_shared<sg::Mesh>();
    sg_mesh->name = mesh.name;

    sg_mesh->primitives.reserve(mesh.primitives.size());

    for (const auto &primitive : mesh.primitives) {
      sg::Primitive sg_primitive;

      // Material
      if (primitive.material >= 0 &&
          static_cast<size_t>(primitive.material) < scene.materials.size()) {
        sg_primitive.material = scene.materials[primitive.material];
      }

      // Positions
      if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
        int accessor_index = primitive.attributes.at("POSITION");
        ExtractAccessorData(model, accessor_index,
                            sg_primitive.attributes.positions);
      }

      // Normals
      if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        int accessor_index = primitive.attributes.at("NORMAL");
        ExtractAccessorData(model, accessor_index,
                            sg_primitive.attributes.normals);
      }

      // Texture coordinates (set 0)
      if (primitive.attributes.find("TEXCOORD_0") !=
          primitive.attributes.end()) {
        int accessor_index = primitive.attributes.at("TEXCOORD_0");
        ExtractAccessorData(model, accessor_index,
                            sg_primitive.attributes.texcoords_0);
      }

      // Texture coordinates (set 1)
      if (primitive.attributes.find("TEXCOORD_1") !=
          primitive.attributes.end()) {
        int accessor_index = primitive.attributes.at("TEXCOORD_1");
        ExtractAccessorData(model, accessor_index,
                            sg_primitive.attributes.texcoords_1);
      }

      // Vertex colors (set 0)
      if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
        int accessor_index = primitive.attributes.at("COLOR_0");
        ExtractAccessorData(model, accessor_index,
                            sg_primitive.attributes.colors_0);
      }

      // Tangents
      if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
        int accessor_index = primitive.attributes.at("TANGENT");
        ExtractAccessorData(model, accessor_index,
                            sg_primitive.attributes.tangents);
      }

      // Indices
      if (primitive.indices >= 0) {
        ExtractAccessorData(model, primitive.indices, sg_primitive.indices);
      }

      // Primitive mode
      sg_primitive.mode = static_cast<sg::Primitive::Mode>(primitive.mode);

      sg_mesh->primitives.push_back(std::move(sg_primitive));
    }

    scene.meshes.push_back(std::move(sg_mesh));
  }
}

void GltfLoader::ProcessNodes(
    const tinygltf::Model &model, sg::Scene &scene,
    std::vector<std::shared_ptr<sg::Node>> &temp_nodes) {
  temp_nodes.reserve(model.nodes.size());

  for (const auto &node : model.nodes) {
    auto sg_node = std::make_shared<sg::Node>();
    sg_node->name = node.name;

    // Link to mesh
    if (node.mesh >= 0 &&
        static_cast<size_t>(node.mesh) < scene.meshes.size()) {
      sg_node->mesh = scene.meshes[node.mesh];
    }

    // Translation
    if (node.translation.size() == 3) {
      sg_node->translation = glm::vec3(static_cast<float>(node.translation[0]),
                                       static_cast<float>(node.translation[1]),
                                       static_cast<float>(node.translation[2]));
      sg_node->use_trs = true;
    }

    // Rotation (quaternion: x, y, z, w in glTF, but glm::quat is w, x, y, z)
    if (node.rotation.size() == 4) {
      sg_node->rotation = glm::quat(static_cast<float>(node.rotation[3]),  // w
                                    static_cast<float>(node.rotation[0]),  // x
                                    static_cast<float>(node.rotation[1]),  // y
                                    static_cast<float>(node.rotation[2])); // z
      sg_node->use_trs = true;
    }

    // Scale
    if (node.scale.size() == 3) {
      sg_node->scale = glm::vec3(static_cast<float>(node.scale[0]),
                                 static_cast<float>(node.scale[1]),
                                 static_cast<float>(node.scale[2]));
      sg_node->use_trs = true;
    }

    // Matrix
    if (node.matrix.size() == 16) {
      sg_node->matrix = glm::make_mat4(node.matrix.data());
      sg_node->use_trs = false;
    }

    temp_nodes.push_back(sg_node);
  }

  // Build hierarchy (second pass)
  for (size_t i = 0; i < model.nodes.size(); ++i) {
    const auto &node = model.nodes[i];
    for (int child_index : node.children) {
      if (child_index >= 0 &&
          static_cast<size_t>(child_index) < temp_nodes.size()) {
        temp_nodes[i]->AddChild(temp_nodes[child_index]);
      }
    }
  }
}

void GltfLoader::ProcessScenes(
    const tinygltf::Model &model, sg::Scene &scene,
    const std::vector<std::shared_ptr<sg::Node>> &temp_nodes) {
  // Use default scene or first scene
  int scene_index = model.defaultScene >= 0 ? model.defaultScene : 0;

  if (scene_index >= 0 &&
      static_cast<size_t>(scene_index) < model.scenes.size()) {
    const auto &gltf_scene = model.scenes[scene_index];
    scene.name = gltf_scene.name;

    // Add root nodes
    for (int node_index : gltf_scene.nodes) {
      if (node_index >= 0 &&
          static_cast<size_t>(node_index) < temp_nodes.size()) {
        scene.root_nodes.push_back(temp_nodes[node_index]);
      }
    }
  }
}

// ============================================================================
// Template specializations for accessor data extraction
// ============================================================================

template <>
void GltfLoader::ExtractAccessorData<glm::vec3>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<glm::vec3> &out_data) {
  if (accessor_index < 0 ||
      static_cast<size_t>(accessor_index) >= model.accessors.size()) {
    return;
  }

  const auto &accessor = model.accessors[accessor_index];

  // Check if bufferView is valid
  if (accessor.bufferView < 0 ||
      static_cast<size_t>(accessor.bufferView) >= model.bufferViews.size()) {
    return;
  }

  const auto &bufferView = model.bufferViews[accessor.bufferView];

  // Check if buffer is valid
  if (bufferView.buffer < 0 ||
      static_cast<size_t>(bufferView.buffer) >= model.buffers.size()) {
    return;
  }

  const auto &buffer = model.buffers[bufferView.buffer];

  // Check if buffer data is loaded (external URIs are loaded by tinygltf
  // automatically)
  if (buffer.data.empty()) {
    warning_message_ += "Warning: Buffer data is empty for accessor " +
                        std::to_string(accessor_index) + "\n";
    return;
  }

  const unsigned char *data_ptr =
      buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

  out_data.resize(accessor.count);

  if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    const float *float_ptr = reinterpret_cast<const float *>(data_ptr);
    size_t stride =
        bufferView.byteStride > 0 ? bufferView.byteStride / sizeof(float) : 3;

    for (size_t i = 0; i < accessor.count; ++i) {
      out_data[i] =
          glm::vec3(float_ptr[i * stride + 0], float_ptr[i * stride + 1],
                    float_ptr[i * stride + 2]);
    }
  }
}

template <>
void GltfLoader::ExtractAccessorData<glm::vec2>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<glm::vec2> &out_data) {
  if (accessor_index < 0 ||
      static_cast<size_t>(accessor_index) >= model.accessors.size()) {
    return;
  }

  const auto &accessor = model.accessors[accessor_index];

  if (accessor.bufferView < 0 ||
      static_cast<size_t>(accessor.bufferView) >= model.bufferViews.size()) {
    return;
  }

  const auto &bufferView = model.bufferViews[accessor.bufferView];

  if (bufferView.buffer < 0 ||
      static_cast<size_t>(bufferView.buffer) >= model.buffers.size()) {
    return;
  }

  const auto &buffer = model.buffers[bufferView.buffer];

  if (buffer.data.empty()) {
    warning_message_ += "Warning: Buffer data is empty for accessor " +
                        std::to_string(accessor_index) + "\n";
    return;
  }

  const unsigned char *data_ptr =
      buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

  out_data.resize(accessor.count);

  if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    const float *float_ptr = reinterpret_cast<const float *>(data_ptr);
    size_t stride =
        bufferView.byteStride > 0 ? bufferView.byteStride / sizeof(float) : 2;

    for (size_t i = 0; i < accessor.count; ++i) {
      out_data[i] =
          glm::vec2(float_ptr[i * stride + 0], float_ptr[i * stride + 1]);
    }
  }
}

template <>
void GltfLoader::ExtractAccessorData<glm::vec4>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<glm::vec4> &out_data) {
  if (accessor_index < 0 ||
      static_cast<size_t>(accessor_index) >= model.accessors.size()) {
    return;
  }

  const auto &accessor = model.accessors[accessor_index];

  if (accessor.bufferView < 0 ||
      static_cast<size_t>(accessor.bufferView) >= model.bufferViews.size()) {
    return;
  }

  const auto &bufferView = model.bufferViews[accessor.bufferView];

  if (bufferView.buffer < 0 ||
      static_cast<size_t>(bufferView.buffer) >= model.buffers.size()) {
    return;
  }

  const auto &buffer = model.buffers[bufferView.buffer];

  if (buffer.data.empty()) {
    warning_message_ += "Warning: Buffer data is empty for accessor " +
                        std::to_string(accessor_index) + "\n";
    return;
  }

  const unsigned char *data_ptr =
      buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

  out_data.resize(accessor.count);

  if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    const float *float_ptr = reinterpret_cast<const float *>(data_ptr);
    size_t stride =
        bufferView.byteStride > 0 ? bufferView.byteStride / sizeof(float) : 4;

    for (size_t i = 0; i < accessor.count; ++i) {
      out_data[i] =
          glm::vec4(float_ptr[i * stride + 0], float_ptr[i * stride + 1],
                    float_ptr[i * stride + 2], float_ptr[i * stride + 3]);
    }
  }
}

template <>
void GltfLoader::ExtractAccessorData<uint32_t>(
    const tinygltf::Model &model, int accessor_index,
    std::vector<uint32_t> &out_data) {
  if (accessor_index < 0 ||
      static_cast<size_t>(accessor_index) >= model.accessors.size()) {
    return;
  }

  const auto &accessor = model.accessors[accessor_index];

  if (accessor.bufferView < 0 ||
      static_cast<size_t>(accessor.bufferView) >= model.bufferViews.size()) {
    return;
  }

  const auto &bufferView = model.bufferViews[accessor.bufferView];

  if (bufferView.buffer < 0 ||
      static_cast<size_t>(bufferView.buffer) >= model.buffers.size()) {
    return;
  }

  const auto &buffer = model.buffers[bufferView.buffer];

  if (buffer.data.empty()) {
    warning_message_ += "Warning: Buffer data is empty for accessor " +
                        std::to_string(accessor_index) + "\n";
    return;
  }

  const unsigned char *data_ptr =
      buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

  out_data.resize(accessor.count);

  switch (accessor.componentType) {
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
    const uint16_t *uint16_ptr = reinterpret_cast<const uint16_t *>(data_ptr);
    for (size_t i = 0; i < accessor.count; ++i) {
      out_data[i] = static_cast<uint32_t>(uint16_ptr[i]);
    }
    break;
  }
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
    const uint32_t *uint32_ptr = reinterpret_cast<const uint32_t *>(data_ptr);
    for (size_t i = 0; i < accessor.count; ++i) {
      out_data[i] = uint32_ptr[i];
    }
    break;
  }
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
    const uint8_t *uint8_ptr = reinterpret_cast<const uint8_t *>(data_ptr);
    for (size_t i = 0; i < accessor.count; ++i) {
      out_data[i] = static_cast<uint32_t>(uint8_ptr[i]);
    }
    break;
  }
  default:
    break;
  }
}

} // namespace paimon
