#version 460 core

// Redeclare built-in block required by ARB_separate_shader_objects
out gl_PerVertex {
  vec4 gl_Position;
};

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

layout(location = 0) out vec2 v_texcoord;

void main() {
    v_texcoord = a_texcoord;
    gl_Position = vec4(a_position, 0.0, 1.0);
}
