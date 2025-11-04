#version 460 core

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_texture;

void main() {
    FragColor = texture(u_texture, v_texcoord);
}
