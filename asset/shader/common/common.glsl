// Common shader definitions and utilities
// Include this file in your shaders using: #include <common.glsl>

#ifndef COMMON_GLSL
#define COMMON_GLSL

// Common constants
const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;
const float HALF_PI = 1.57079632679;

// Utility functions
float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

vec3 saturate(vec3 v) {
    return clamp(v, vec3(0.0), vec3(1.0));
}

// Gamma correction
vec3 linearToSRGB(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec3 sRGBToLinear(vec3 color) {
    return pow(color, vec3(2.2));
}

#endif // COMMON_GLSL
