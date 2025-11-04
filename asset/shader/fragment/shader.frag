#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoord;

layout(location = 0, binding = 0) uniform sampler2D u_texture;

void main() 
{
    vec4 texColor = texture(u_texture, TexCoord);
    FragColor = vec4(texColor);
}