#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

#ifdef USE_TRANSFORM
uniform mat4 u_transform;
#endif

void main() {
#ifdef USE_TRANSFORM
    gl_Position = u_transform * vec4(a_position, 1.0);
#else
    gl_Position = vec4(a_position, 1.0);
#endif
    v_color = a_color;
}
