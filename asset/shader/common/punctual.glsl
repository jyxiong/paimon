// Punctual lights support (point lights and spot lights)

#include <common/brdf.glsl>

// Light types
const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT = 1;
const int LIGHT_TYPE_SPOT = 2;

// Structure for punctual lights (point/spot)
struct PunctualLight
{
  vec3 position;
  int type;
  vec3 direction;
  float range;
  vec3 color;
  float intensity;
  float innerConeAngle;
  float outerConeAngle;
  vec2 _padding;
};

// Calculate attenuation for point lights and spot lights
float calculateAttenuation(float distance, float range)
{
  if (range <= 0.0)
  {
    return 1.0; // No attenuation
  }
  
  // Using inverse square law with smooth falloff
  float attenuation = 1.0 / (distance * distance);
  
  // Smooth cutoff at range
  float factor = clamp(1.0 - pow(distance / range, 4.0), 0.0, 1.0);
  factor = factor * factor;
  
  return attenuation * factor;
}

// Calculate spot light effect (cone attenuation)
float calculateSpotEffect(vec3 L, vec3 spotDirection, float innerConeAngle, float outerConeAngle)
{
  vec3 spotDir = normalize(spotDirection);
  float cosAngle = dot(-L, spotDir);
  
  float cosInner = cos(innerConeAngle);
  float cosOuter = cos(outerConeAngle);
  
  // Smooth transition between inner and outer cone
  float spotEffect = smoothstep(cosOuter, cosInner, cosAngle);
  
  return spotEffect;
}


// Calculate light direction vector L for a given light
vec3 calculateLightDirection(PunctualLight light, vec3 fragPosition)
{
  if (light.type == LIGHT_TYPE_DIRECTIONAL)
  {
    return normalize(-light.direction);
  }
  else // LIGHT_TYPE_POINT or LIGHT_TYPE_SPOT
  {
    return normalize(light.position - fragPosition);
  }
}

// Calculate incoming radiance for a given light
vec3 calculateRadiance(PunctualLight light, vec3 fragPosition, vec3 L)
{
  vec3 radiance = light.color * light.intensity;

  if (light.type == LIGHT_TYPE_DIRECTIONAL)
  {
    // No attenuation for directional lights
    return radiance;
  }
  else if (light.type == LIGHT_TYPE_POINT)
  {
    float distance = length(light.position - fragPosition);
    float attenuation = calculateAttenuation(distance, light.range);
    return radiance * attenuation;
  }
  else if (light.type == LIGHT_TYPE_SPOT)
  {
    float distance = length(light.position - fragPosition);
    float attenuation = calculateAttenuation(distance, light.range);
    float spotEffect = calculateSpotEffect(L, light.direction, light.innerConeAngle, light.outerConeAngle);
    return radiance * attenuation * spotEffect;
  }
  
  return vec3(0.0); // Unknown light type
}

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

  // Calculate reflectance at normal incidence
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);

  // Cook-Torrance BRDF
  // V_SmithGGXCorrelated already includes the 4*NdotV*NdotL denominator
  float NDF = DistributionGGX(N, H, roughness);
  float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
  vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 specular = NDF * Vis * F;

  // Energy conservation: kS + kD = 1.0
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic; // Metallic surfaces don't have diffuse

  return (kD * albedo / PI + specular) * radiance * NdotL;
}

