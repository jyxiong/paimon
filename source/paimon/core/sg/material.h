#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

namespace paimon {
namespace sg {

struct Texture;

/// Material definition supporting PBR workflow
struct Material {
  std::string name;

  /// PBR metallic-roughness material parameters
  struct PbrMetallicRoughness {
    glm::vec4 base_color_factor = glm::vec4(1.0f); // RGBA
    std::shared_ptr<Texture> base_color_texture = nullptr;
    int base_color_tex_coord = 0; // Texture coordinate set

    float metallic_factor = 1.0f;
    float roughness_factor = 1.0f;
    std::shared_ptr<Texture> metallic_roughness_texture = nullptr;
    int metallic_roughness_tex_coord = 0;
  };

  // PBR metallic-roughness
  PbrMetallicRoughness pbr_metallic_roughness;

  // Normal mapping
  std::shared_ptr<Texture> normal_texture = nullptr;
  int normal_tex_coord = 0;
  float normal_scale = 1.0f;

  // Occlusion mapping
  std::shared_ptr<Texture> occlusion_texture = nullptr;
  int occlusion_tex_coord = 0;
  float occlusion_strength = 1.0f;

  // Emissive
  std::shared_ptr<Texture> emissive_texture = nullptr;
  int emissive_tex_coord = 0;
  glm::vec3 emissive_factor = glm::vec3(0.0f);

  // Alpha mode
  enum class AlphaMode { Opaque, Mask, Blend };
  AlphaMode alpha_mode = AlphaMode::Opaque;
  float alpha_cutoff = 0.5f;

  bool double_sided = false;

  // Extensions (glTF 2.0 official extensions)
  
  /// KHR_materials_anisotropy
  struct Anisotropy {
    float anisotropyStrength = 0.0f;  // Strength of anisotropy [0.0, 1.0]
    float anisotropyRotation = 0.0f;  // Rotation in radians [0.0, 2π]
    std::shared_ptr<Texture> anisotropyTexture = nullptr;  // RG channels: strength (R) + rotation (G)
    int anisotropyTexCoord = 0;

    bool IsEnabled() const {
      return anisotropyStrength > 0.0f || anisotropyTexture != nullptr;
    }
  };
  Anisotropy anisotropy;

  /// KHR_materials_clearcoat
  struct Clearcoat {
    float clearcoatFactor;
    std::shared_ptr<Texture> clearcoatTexture;
    float clearcoatRoughnessFactor;
    std::shared_ptr<Texture> clearcoatRoughnessTexture;
    std::shared_ptr<Texture> clearcoatNormalTexture;
    int clearcoatTexCoord;
    int clearcoatRoughnessTexCoord;
    int clearcoatNormalTexCoord;
  };


  /// KHR_materials_dispersion
  /// KHR_materials_emissive_strength
  /// KHR_materials_ior
  /// KHR_materials_iridescence
  /// KHR_materials_sheen
  /// KHR_materials_specular
  /// KHR_materials_transmission

  /// KHR_materials_unlit
  bool unlit = false;

  /// KHR_materials_variants
  // KHR_materials_volume

  Material() = default;
  Material(const std::string &name) : name(name) {}
};

} // namespace sg
} // namespace paimon
