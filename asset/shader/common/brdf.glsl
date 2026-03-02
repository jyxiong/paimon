// PBR (Physically Based Rendering) common functions
// Based on Cook-Torrance BRDF

const float PI = 3.14159265359;

// Fresnel-Schlick (general form, roughness clamps the max reflectance for IBL)
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
  vec3 Fmax = max(vec3(1.0 - roughness), F0);
  return F0 + (Fmax - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Convenience overload for direct lighting (roughness = 0)
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
  return fresnelSchlick(cosTheta, F0, 0.0);
}

// GGX/Trowbridge-Reitz normal distribution function
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / max(denom, 0.0001);
}

// Height-Correlated Smith GGX visibility term (Heitz 2014, used by Filament/Unity HDRP)
// Returns V = G / (4 * NdotV * NdotL) — denominator already incorporated
float V_SmithGGXCorrelated(float NdotV, float NdotL, float roughness)
{
  float a2 = roughness * roughness * roughness * roughness;
  float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - a2) + a2);
  float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - a2) + a2);
  return 0.5 / max(GGXV + GGXL, 0.0001);
}
