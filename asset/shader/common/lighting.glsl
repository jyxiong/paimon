// Common lighting functions

#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

vec3 calculateLighting(vec3 normal, vec3 fragPos, Light light) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.color * light.intensity * diff;
}

#ifdef ENABLE_SHADOWS
float calculateShadow(vec3 fragPos, Light light) {
    // Simple shadow calculation
    float distance = length(light.position - fragPos);
    return smoothstep(0.0, 1.0, distance / 10.0);
}
#endif

#endif // LIGHTING_GLSL
