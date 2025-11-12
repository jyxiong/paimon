#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
  v_position = vec3(u_model * vec4(a_position, 1.0));
  v_normal = mat3(transpose(inverse(u_model))) * a_normal;
  v_texcoord = a_texcoord;
  gl_Position = u_projection * u_view * vec4(v_position, 1.0);
}
