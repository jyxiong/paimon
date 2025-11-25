#version 460 core

in vec2 v_texcoord;

out vec4 FragColor;

layout(binding = 6) uniform sampler2D u_screen_texture;

void main()
{
    FragColor = texture(u_screen_texture, v_texcoord);
}
