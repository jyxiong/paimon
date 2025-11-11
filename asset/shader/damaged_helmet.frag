#version 460 core

#include <pbr.glsl>

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 FragColor;

uniform sampler2D u_baseColorTexture;
uniform sampler2D u_metallicRoughnessTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_emissiveTexture;
uniform sampler2D u_occlusionTexture;

uniform vec4 u_baseColorFactor;
uniform float u_metallicFactor;
uniform float u_roughnessFactor;
uniform vec3 u_emissiveFactor;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

void main()
{
  // Sample textures
  vec4 baseColor = texture(u_baseColorTexture, v_texcoord) * u_baseColorFactor;
  vec4 metallicRoughness = texture(u_metallicRoughnessTexture, v_texcoord);
  float metallic = metallicRoughness.b * u_metallicFactor;
  float roughness = metallicRoughness.g * u_roughnessFactor;
  vec3 emissive = texture(u_emissiveTexture, v_texcoord).rgb * u_emissiveFactor;
  float ao = texture(u_occlusionTexture, v_texcoord).r;

  // Normal from normal map
  vec3 N = normalize(v_normal);
  vec3 V = normalize(u_viewPos - v_position);
  vec3 L = normalize(u_lightPos - v_position);

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

  FragColor = vec4(color, baseColor.a);
}
