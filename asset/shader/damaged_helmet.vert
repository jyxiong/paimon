#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;

// Redeclare built-in block required by ARB_separate_shader_objects
out gl_PerVertex {
  vec4 gl_Position;
};

// UBO for transformation matrices
layout(std140, binding = 0) uniform TransformUBO
{
  mat4 model;
  mat4 view;
  mat4 projection;
} u_transform;

void main()
{
  v_position = vec3(u_transform.model * vec4(a_position, 1.0));
  v_normal = mat3(transpose(inverse(u_transform.model))) * a_normal;
  v_texcoord = a_texcoord;
  // Compute clip-space position directly from model * position to avoid
  // any possible mismatch between varying and clip-space transforms.
  gl_Position = u_transform.projection * u_transform.view * (u_transform.model * vec4(a_position, 1.0));
}
