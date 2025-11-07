// Common utility functions

#ifndef UTILS_GLSL
#define UTILS_GLSL

// Gamma correction
vec3 toLinear(vec3 color) {
    return pow(color, vec3(2.2));
}

vec3 toGamma(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

// Simple noise function
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

#endif // UTILS_GLSL
