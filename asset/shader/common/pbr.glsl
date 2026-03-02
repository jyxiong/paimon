// PBR lighting calculation
// Combines BRDF functions with light contribution

#include <common/brdf.glsl>

// Calculate PBR lighting for a single light source
// Returns the contribution from this light
vec3 calculatePBRLighting(
  vec3 N,           // Normal
  vec3 V,           // View direction
  vec3 L,           // Light direction
  vec3 albedo,      // Base color
  float metallic,   // Metallic value
  float roughness,  // Roughness value
  vec3 radiance     // Light color/intensity
)
{
  vec3 H = normalize(V + L);

  vec3 F0 = mix(vec3(0.04), albedo, metallic);

  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);

  // Cook-Torrance BRDF
  // V_SmithGGXCorrelated already includes the 4*NdotV*NdotL denominator
  float NDF = DistributionGGX(N, H, roughness);
  float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
  vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 specular = NDF * Vis * F;

  // Energy conservation: kS + kD = 1.0
  vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

  return (kD * albedo / PI + specular) * radiance * NdotL;
}
