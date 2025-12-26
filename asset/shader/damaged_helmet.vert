#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in vec3 a_color;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;
out vec3 v_color;

// Redeclare built-in block required by ARB_separate_shader_objects
out gl_PerVertex {
  vec4 gl_Position;
};

// UBO for transformation matrices
layout(std140, binding = 0) uniform TransformUBO
{
  mat4 model;
} u_transform;

// UBO for camera
layout(std140, binding = 1) uniform CameraUBO
{
  mat4 view;
  mat4 projection;
  vec3 position;
} u_camera;

void main()
{
  vec4 worldPos = u_transform.model * vec4(a_position, 1.0);
  v_position = worldPos.xyz;
  v_normal = mat3(transpose(inverse(u_transform.model))) * a_normal;
  v_texcoord = a_texcoord;
  v_color = a_color;
  
  gl_Position = u_camera.projection * u_camera.view * worldPos;
}
