// Punctual lights support (point lights and spot lights)

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
