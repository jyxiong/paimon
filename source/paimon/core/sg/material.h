#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "paimon/core/sg/texture.h"

namespace paimon {
namespace sg {

enum class AlphaMode {
   Opaque = 0, 
   Mask, 
   Blend ,
   Count,
};

/// PBR metallic-roughness material parameters
struct PbrMetallicRoughness {
  glm::vec4 baseColorFactor = glm::vec4(1.0f); // RGBA
  std::shared_ptr<Texture> baseColorTexture;
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;
  std::shared_ptr<Texture> metallicRoughnessTexture;
};

/// KHR_materials_anisotropy
struct Anisotropy {
  float strength = 0.0f;
  float rotation = 0.0f;
  std::shared_ptr<Texture> texture;
};

/// KHR_materials_clearcoat
struct Clearcoat {
  float factor;
  std::shared_ptr<Texture> texture;
  float roughnessFactor;
  std::shared_ptr<Texture> roughnessTexture;
  std::shared_ptr<Texture> normalTexture = nullptr;
  float normalScale = 1.0f;
};

/// KHR_materials_dispersion
/// KHR_materials_emissive_strength
/// KHR_materials_ior
/// KHR_materials_iridescence
/// KHR_materials_sheen
/// KHR_materials_specular
/// KHR_materials_transmission
/// KHR_materials_unlit
/// KHR_materials_variants
/// KHR_materials_volume

/// Material definition supporting PBR workflow
struct Material {

  // PBR metallic-roughness
  PbrMetallicRoughness pbrMetallicRoughness;

  // Normal mapping
  std::shared_ptr<Texture> normalTexture = nullptr;
  float normalScale = 1.0f;

  // Occlusion mapping
  std::shared_ptr<Texture> occlusionTexture = nullptr;
  float occlusionStrength = 1.0f;

  // Emissive
  std::shared_ptr<Texture> emissiveTexture;
  glm::vec3 emissiveFactor = glm::vec3(0.0f);

  // Alpha mode
  AlphaMode alphaMode = AlphaMode::Opaque;

  float alphaCutoff = 0.5f;

  bool doubleSided = false;

  Anisotropy anisotropy;

  Clearcoat clearcoat;
};

} // namespace sg
} // namespace paimon
