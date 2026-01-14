#version 460 core

#include <brdf.glsl>
#include <punctual.glsl>

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_color;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_baseColorTexture;
layout(binding = 1) uniform sampler2D u_metallicRoughnessTexture;
layout(binding = 2) uniform sampler2D u_normalTexture;
layout(binding = 3) uniform sampler2D u_emissiveTexture;
layout(binding = 4) uniform sampler2D u_occlusionTexture;

// UBO for camera
layout(std140, binding = 1) uniform CameraUBO
{
  mat4 view;
  mat4 projection;
  vec3 position;
} u_camera;

// UBO for material properties
layout(std140, binding = 2) uniform MaterialUBO
{
  vec4 baseColorFactor;
  vec3 emissiveFactor;
  float metallicFactor;
  float roughnessFactor;
  float _padding[3]; // alignment
} u_material;

// UBO for all lighting (fixed maximum size)
layout(std140, binding = 3) uniform LightingUBO
{
  int lightCount;
  int _padding[3];
  PunctualLight lights[32]; // Maximum 32 lights
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
  vec3 V = normalize(u_camera.position - v_position);

  // Initialize lighting accumulation
  vec3 Lo = vec3(0.0);

  // Calculate lighting contribution from all lights
  for (int i = 0; i < u_lighting.lightCount; ++i)
  {
    PunctualLight light = u_lighting.lights[i];
    vec3 L = calculateLightDirection(light, v_position);
    vec3 radiance = calculateRadiance(light, v_position, L);
    Lo += calculatePBRLighting(N, V, L, baseColor.rgb, metallic, roughness, radiance);
  }

  // Ambient lighting (simplified)
  vec3 ambient = vec3(0.03) * baseColor.rgb * ao;

  vec3 color = ambient + Lo + emissive;

  // Tone mapping (simple Reinhard)
  color = color / (color + vec3(1.0));
  // Gamma correction
  color = pow(color, vec3(1.0 / 2.2));

  FragColor = vec4(color, 1.0);
}
