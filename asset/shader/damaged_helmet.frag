#version 460 core

#include <pbr.glsl>

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_baseColorTexture;
layout(binding = 1) uniform sampler2D u_metallicRoughnessTexture;
layout(binding = 2) uniform sampler2D u_normalTexture;
layout(binding = 3) uniform sampler2D u_emissiveTexture;
layout(binding = 4) uniform sampler2D u_occlusionTexture;

// UBO for material properties
layout(std140, binding = 1) uniform MaterialUBO
{
  vec4 baseColorFactor;
  vec3 emissiveFactor;
  float metallicFactor;
  float roughnessFactor;
  float _padding[3]; // alignment
} u_material;

// UBO for lighting
layout(std140, binding = 2) uniform LightingUBO
{
  vec3 lightPos;
  float _padding1;
  vec3 viewPos;
  float _padding2;
} u_lighting;

void main()
{
  // Sample textures
  vec4 baseColor = texture(u_baseColorTexture, v_texcoord) * u_material.baseColorFactor;
  vec4 metallicRoughness = texture(u_metallicRoughnessTexture, v_texcoord);
  float metallic = metallicRoughness.b * u_material.metallicFactor;
  float roughness = metallicRoughness.g * u_material.roughnessFactor;
  vec3 emissive = texture(u_emissiveTexture, v_texcoord).rgb * u_material.emissiveFactor;
  float ao = texture(u_occlusionTexture, v_texcoord).r;

  // Normal from normal map
  vec3 N = normalize(v_normal);
  vec3 V = normalize(u_lighting.viewPos - v_position);
  vec3 L = normalize(u_lighting.lightPos - v_position);

  // Calculate PBR lighting
  vec3 radiance = vec3(1.0); // Light color/intensity
  vec3 Lo = calculatePBRLighting(N, V, L, baseColor.rgb, metallic, roughness, radiance);

  // Ambient lighting (simplified)
  vec3 ambient = vec3(0.03) * baseColor.rgb * ao;

  vec3 color = ambient + Lo + emissive;

  // Tone mapping (simple Reinhard)
  color = color / (color + vec3(1.0));
  // Gamma correction
  color = pow(color, vec3(1.0 / 2.2));

  FragColor = vec4(color, 1.0);
}
