#include <cmath>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/app/window.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program_pipeline.h"
#include "paimon/opengl/shader_program.h"
#include "paimon/opengl/vertex_array.h"

using namespace paimon;

std::string vertex_shader = R"(
#version 460 core
layout(location = 0) in vec3 in_pos;
layout(location = 1) in float in_width;

out VS_OUT {
  vec3 pos;
  float width;
} vs_out;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  vs_out.pos = in_pos;
  vs_out.width = in_width;
})";

std::string geometry_shader = R"(
#version 460 core
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT {
  vec3 pos;
  float width;
} gs_in[];

out vec3 frag_pos;
// For cylinder-like shading we output the spine position (center of the segment),
// the line direction and the half width so the fragment shader can compute a
// radial normal and a falloff from the spine to the edge.
out vec3 spine_pos_out;
out vec3 line_dir_out;
out float half_width_out;

out gl_PerVertex {
  vec4 gl_Position;
};

layout(std140, binding = 0) uniform PerFrame {
  mat4 u_view;
  mat4 u_proj;
  vec4 u_camera_pos;
};

// 用于生成线带的法线（摄像机朝向为简化）
vec3 getNormal(vec3 p0, vec3 p1) {
  vec3 view_dir = normalize(u_camera_pos.xyz - p0);
  vec3 line_dir = normalize(p1 - p0);
  return normalize(cross(line_dir, view_dir));
}

void main() {
  vec3 p0 = gs_in[0].pos;
  vec3 p1 = gs_in[1].pos;
  float w0 = gs_in[0].width * 0.5;
  float w1 = gs_in[1].width * 0.5;

  vec3 n = getNormal(p0, p1);

  // 两端扩展为四个顶点
  vec3 v0 = p0 + n * w0;
  vec3 v1 = p0 - n * w0;
  vec3 v2 = p1 + n * w1;
  vec3 v3 = p1 - n * w1;

  // 输出三角带
  frag_pos = v0;
  spine_pos_out = p0;
  line_dir_out = p1 - p0;
  half_width_out = w0;
  gl_Position = u_proj * u_view * vec4(v0, 1.0);
  
  EmitVertex();

  frag_pos = v1;
  spine_pos_out = p0;
  line_dir_out = p1 - p0;
  half_width_out = w0;
  gl_Position = u_proj * u_view * vec4(v1, 1.0);

  EmitVertex();

  frag_pos = v2;
  spine_pos_out = p1;
  line_dir_out = p1 - p0;
  half_width_out = w1;
  gl_Position = u_proj * u_view * vec4(v2, 1.0);

  EmitVertex();

  frag_pos = v3;
  spine_pos_out = p1;
  line_dir_out = p1 - p0;
  half_width_out = w1;
  gl_Position = u_proj * u_view * vec4(v3, 1.0);

  EmitVertex();

  EndPrimitive();
}
)";

std::string fragment_shader = R"(
#version 460 core
in vec3 frag_pos;
in vec3 spine_pos_out;
in vec3 line_dir_out;
in float half_width_out;
out vec4 out_color;

layout(std140, binding = 1) uniform Lighting {
  vec4 u_light_pos;
  vec4 u_light_color;
  vec4 u_view_pos;
  vec4 u_obj_color;
};

void main() {
  // compute local basis: dir is tangent (center line), across is vector from spine to fragment
  vec3 dir = normalize(line_dir_out);
  vec3 to_frag = frag_pos - spine_pos_out;
  vec3 proj = to_frag - dir * dot(to_frag, dir);
  float dist = length(proj);

  // choose a safe up vector that is not parallel to dir
  vec3 view_dir = normalize(u_view_pos.xyz - frag_pos); // per-fragment view direction
  vec3 up = (abs(dir.y) < 0.99) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
  // prefer projecting to_frag to get across; if degenerate, use cross(dir, view_dir)
  vec3 across = vec3(0.0);
  if (dist > 1e-6) {
    across = normalize(proj);
  } else {
    // fallback uses view_dir rather than a constant up to keep orientation stable
    across = normalize(cross(dir, view_dir));
    // if cross(dir, view_dir) is near-zero (dir ~ view_dir) fallback to constant up
    if (length(across) < 1e-6) {
      across = normalize(cross(dir, up));
    }
  }

  // rectangle normal: perpendicular to both tangent and across (i.e., out-of-ribbon)
  vec3 rect_normal = normalize(cross(dir, across));

  // lighting
  vec3 L = normalize(u_light_pos.xyz - frag_pos);
  float diff = dot(rect_normal, L);
  if (diff < 0.0) {
    diff = -diff; // flip to ensure positive contribution
  }
  
  float t = clamp(dist / half_width_out, 0.0, 1.0);
  float falloff = cos(t * 1.57079632679); // cos(pi/2 * t)

  vec3 diffuse = diff * u_light_color.xyz * u_obj_color.xyz * falloff;
  out_color = vec4(diffuse, 1.0);
}

 )";

glm::ivec2 g_size = {};

int main() {
  LogSystem::init();

  auto window = Window::create(WindowConfig{
      .title = "Geometry Shader Example",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
          },
      .width = 800,
      .height = 600,
  });

  // enable depth test for correct 3D rasterization
  glEnable(GL_DEPTH_TEST);

  // 创建着色器程序
  ShaderProgram vertex_program(GL_VERTEX_SHADER, vertex_shader);
  ShaderProgram geometry_program(GL_GEOMETRY_SHADER, geometry_shader);
  ShaderProgram fragment_program(GL_FRAGMENT_SHADER, fragment_shader);
  if (!vertex_program.is_valid() || !geometry_program.is_valid() ||
      !fragment_program.is_valid()) {
    LOG_ERROR("Failed to create shader programs");
    // print info logs to help debugging
    auto vlog = vertex_program.get_info_log();
    if (!vlog.empty())
      LOG_ERROR("Vertex program log:\n{}", vlog);
    auto g_log = geometry_program.get_info_log();
    if (!g_log.empty())
      LOG_ERROR("Geometry program log:\n{}", g_log);
    auto f_log = fragment_program.get_info_log();
    if (!f_log.empty())
      LOG_ERROR("Fragment program log:\n{}", f_log);
    return EXIT_FAILURE;
  }

  ProgramPipeline pipeline;
  pipeline.use_program_stages(GL_VERTEX_SHADER_BIT, vertex_program.get_name());
  pipeline.use_program_stages(GL_GEOMETRY_SHADER_BIT,
                              geometry_program.get_name());
  pipeline.use_program_stages(GL_FRAGMENT_SHADER_BIT,
                              fragment_program.get_name());
  if (!pipeline.is_valid()) {
    LOG_ERROR("Failed to create program pipeline");
    return EXIT_FAILURE;
  }

  // Print program info logs (useful if shaders compiled but linked with
  // warnings)
  auto vlog = vertex_program.get_info_log();
  if (!vlog.empty())
    LOG_INFO("Vertex program log:\n{}", vlog);
  auto glog = geometry_program.get_info_log();
  if (!glog.empty())
    LOG_INFO("Geometry program log:\n{}", glog);
  auto flog = fragment_program.get_info_log();
  if (!flog.empty())
    LOG_INFO("Fragment program log:\n{}", flog);

  // Validate pipeline
  if (!pipeline.validate()) {
    LOG_WARN("Program pipeline validation failed");
  }

  // --- prepare some default camera / lighting uniforms ---
  // Camera
  glm::mat4 view =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  float aspect = (g_size.y == 0) ? 1.0f : float(g_size.x) / float(g_size.y);
  glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

  // Lighting / object
  glm::vec3 light_pos(1.0f, 1.0f, 2.0f);
  glm::vec3 light_color(1.0f, 1.0f, 1.0f);
  glm::vec3 view_pos(0.0f, 0.0f, 2.0f);
  glm::vec3 obj_color(0.8f, 0.4f, 0.2f);
  // Instead of setting individual uniforms, create two UBOs:
  // - PerFrame (binding = 0): view and proj matrices
  // - Lighting (binding = 1): light position/color, view pos, object color

  struct PerFrameUBO {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 camera_pos;
  } perFrameData{view, proj};

  struct LightingUBO {
    glm::vec4 light_pos;
    glm::vec4 light_color;
    glm::vec4 view_pos;
    glm::vec4 obj_color;
  } lightingData{glm::vec4(light_pos, 1.0f), glm::vec4(light_color, 0.0f),
                 glm::vec4(view_pos, 0.0f), glm::vec4(obj_color, 0.0f)};

  // create UBOs and bind to binding points
  Buffer perFrameUBOBuffer, lightingUBOBuffer;
  // ------------------------------------------------------
  perFrameUBOBuffer.set_data(0, sizeof(PerFrameUBO), &perFrameData);
  if (!perFrameUBOBuffer.is_valid()) {
    LOG_ERROR("Failed to create per-frame UBO");
    return EXIT_FAILURE;
  }

  lightingUBOBuffer.set_data(0, sizeof(LightingUBO), &lightingData);
  if (!lightingUBOBuffer.is_valid()) {
    LOG_ERROR("Failed to create lighting UBO");
    return EXIT_FAILURE;
  }
  // bind UBOs to binding points (while no VAO is bound)
  perFrameUBOBuffer.bind_base(GL_UNIFORM_BUFFER, 0);
  lightingUBOBuffer.bind_base(GL_UNIFORM_BUFFER, 1);

  // Debug: check uniform buffer binding points
  GLint bound0 = 0, bound1 = 0;
  glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, 0, &bound0);
  glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, 1, &bound1);
  LOG_INFO("UBO binding 0 -> buffer {}, binding 1 -> buffer {}", bound0,
           bound1);

  // ------------------------------------------------------

  // 创建顶点数据
  struct Vertex {
    float pos[3];
    float width;
  };

  // 生成十个顶点，组成一条平滑的曲线（sine-based）
  const int pointCount = 20;
  std::vector<Vertex> vertices;
  vertices.reserve(pointCount);
  const float startX = -0.8f;
  const float endX = 1.2f;
  const float amplitude = 0.6f;
  const float frequency = 1.5f; // number of sine cycles across the curve
  for (int i = 0; i < pointCount; ++i) {
    float t = (pointCount == 1) ? 0.0f : float(i) / float(pointCount - 1);
    float x = startX + (endX - startX) * t;
    float y = std::sin(t * 2.0f * 3.14159265f * frequency) * amplitude;
    // small tapering to make endpoints less extreme
    y *= (0.8f + 0.2f * (1.0f - std::abs(2.0f * t - 1.0f)));
    float width = 0.2f;
    vertices.push_back(Vertex{{x, y, 0.0f}, width});
  }

  Buffer vbo;
  vbo.set_storage(sizeof(Vertex) * vertices.size(), vertices.data());
  if (!vbo.is_valid()) {
    LOG_ERROR("Failed to create vertex buffer");
    return EXIT_FAILURE;
  }
  vbo.bind(GL_ARRAY_BUFFER);

  // create element buffer (pairs of indices to form GL_LINES segments)
  // create element buffer (pairs of indices to form GL_LINES segments)
  std::vector<unsigned int> indices;
  indices.reserve((pointCount - 1) * 2);
  for (int i = 0; i < pointCount - 1; ++i) {
    indices.push_back(i);
    indices.push_back(i + 1);
  }
  Buffer ebo;
  ebo.set_storage(sizeof(unsigned int) * indices.size(), indices.data());
  if (!ebo.is_valid()) {
    LOG_ERROR("Failed to create element buffer");
    return EXIT_FAILURE;
  }
  ebo.bind(GL_ELEMENT_ARRAY_BUFFER);

  // 创建顶点数组对象
  VertexArray vao;
  vao.bind();

  auto &binding = vao.get_binding(0);
  binding.bind_buffer(vbo, 0, sizeof(Vertex));

  auto &pos_attribute = vao.get_attribute(0);
  pos_attribute.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
  pos_attribute.bind(binding);
  pos_attribute.enable();

  auto &width_attribute = vao.get_attribute(1);
  width_attribute.set_format(1, GL_FLOAT, GL_FALSE, offsetof(Vertex, width));
  width_attribute.bind(binding);
  width_attribute.enable();

  vao.set_element_buffer(ebo);
  if (!vao.is_valid()) {
    LOG_ERROR("Failed to create vertex array object");
    return EXIT_FAILURE;
  }

  while (!window->shouldClose()) {
    // Render here
    glViewport(0, 0, g_size.x, g_size.y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If camera/projection can change, update per-frame UBO each frame
    perFrameData.view = view;
    float aspect = (g_size.y == 0) ? 1.0f : float(g_size.x) / float(g_size.y);
    perFrameData.proj =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    perFrameData.camera_pos = glm::vec4(view_pos, 1.0f);
    perFrameUBOBuffer.set_sub_data(0, sizeof(PerFrameUBO), &perFrameData);

    pipeline.bind();
    vao.bind();

    // draw as lines so geometry shader receives 'lines' primitives
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, 0);

    // Swap front and back buffers
    window->swapBuffers();

    // Poll for and process events
    window->pollEvents();
  }

  // cleanup glfw
  window->destroy();

  return 0;
}