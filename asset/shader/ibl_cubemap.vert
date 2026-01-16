#version 460 core

// Redeclare built-in block required by ARB_separate_shader_objects
out gl_PerVertex {
  vec4 gl_Position;
};

layout(location = 0) in vec3 a_position;

layout(location = 0) out vec3 v_localPos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    v_localPos = a_position;
    gl_Position = u_projection * u_view * vec4(a_position, 1.0);
}
