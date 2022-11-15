#version 330 core

layout(location = 0) out vec4 o_fragColor;

uniform sampler2D u_diffuse_texture;

in vec4 v_color;
in vec2 v_uv;

void main()
{
    vec4 pixColor = texture(u_diffuse_texture, v_uv);
    o_fragColor = vec4(v_color.rgb, pixColor.r * v_color.a);
}
