#version 460 core

#include <common/utils.glsl>

in vec3 v_color;
out vec4 FragColor;

void main() {
    vec3 color = v_color;
    
#ifdef ENABLE_GAMMA_CORRECTION
    // Apply gamma correction using utility function
    color = toGamma(color);
#endif

#ifdef ENABLE_BRIGHTNESS
    // Increase brightness
    color *= BRIGHTNESS_FACTOR;
#endif
    
    FragColor = vec4(color, 1.0);
}
