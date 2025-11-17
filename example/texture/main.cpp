
#include <glm/glm.hpp>

#include "paimon/core/log_system.h"

#include "paimon/app/window.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"

using namespace paimon;

namespace {
// hard code triangle vertices in shader for simplicity
std::string vertex_source = R"(
  #version 460 core

  out vec2 TexCoord;

  layout(location = 0) in vec3 a_position;
  layout(location = 1) in vec2 a_texcoord;

  void main()
  {
    gl_Position = vec4(a_position, 1.0);
    TexCoord = a_texcoord;
  }
  )";

std::string fragment_source = R"(
  #version 460 core

  in vec2 TexCoord;

  out vec4 FragColor;

  layout(location = 0) uniform sampler2D u_texture;

  void main() 
  {
    vec4 texColor = texture(u_texture, TexCoord);
    FragColor = vec4(texColor);
  }
  )";

glm::ivec2 g_size = {};
} // namespace

int main() {
  LogSystem::init();

  auto window = Window::create(WindowConfig{
      .title = "globjects Texture",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = 640,
      .height = 480,
      .resizable = false,
      .visible = true,
      .vsync = false,
  });

  Shader vertex_shader(GL_VERTEX_SHADER);
  Shader fragment_shader(GL_FRAGMENT_SHADER);
  Program program;

  if (!vertex_shader.compile(vertex_source)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertex_shader.get_info_log());
  }

  if (!fragment_shader.compile(fragment_source)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragment_shader.get_info_log());
  }

  program.attach(vertex_shader);
  program.attach(fragment_shader);
  if (!program.link()) {
    LOG_ERROR("Shader program linking failed: {}", program.get_info_log());
  }

  // Interleaved vertex data: position (vec3) + texcoord (vec2)
  std::vector<glm::vec3> positions = {
      {-0.5f, -0.5f, 0.0f}, // Bottom left
      {0.5f, -0.5f, 0.0f},  // Bottom right
      {-0.5f, 0.5f, 0.0f},  // Top left
      {0.5f, 0.5f, 0.0f}    // Top right
  };

  std::vector<glm::vec2> texcoords = {
      {0.0f, 0.0f}, // Bottom left
      {1.0f, 0.0f}, // Bottom right
      {0.0f, 1.0f}, // Top left
      {1.0f, 1.0f}  // Top right
  };

  std::vector<unsigned int> indices = {
      0, 1, 2, // First triangle
      1, 3, 2  // Second triangle
  };

  Buffer pos_vbo;
  pos_vbo.set_storage(sizeof(glm::vec3) * positions.size(), positions.data(),
                      GL_DYNAMIC_STORAGE_BIT);
  pos_vbo.bind(GL_ARRAY_BUFFER);

  Buffer tex_vbo;
  tex_vbo.set_storage(sizeof(glm::vec2) * texcoords.size(), texcoords.data(),
                      GL_DYNAMIC_STORAGE_BIT);
  tex_vbo.bind(GL_ARRAY_BUFFER);

  Buffer ebo;
  ebo.set_storage(sizeof(unsigned int) * indices.size(), indices.data(),
                  GL_DYNAMIC_STORAGE_BIT);
  ebo.bind(GL_ELEMENT_ARRAY_BUFFER);

  VertexArray vao;
  vao.bind();
  auto &binding_pos = vao.get_binding(0);
  binding_pos.bind_buffer(
      pos_vbo, 0, sizeof(glm::vec3)); // stride = sizeof(glm::vec3), offset = 0
  auto &binding_tex = vao.get_binding(1);
  binding_tex.bind_buffer(tex_vbo, 0, sizeof(glm::vec2));

  // 位置属性
  auto &attribute_pos = vao.get_attribute(0);
  attribute_pos.set_format(3, GL_FLOAT, GL_FALSE, 0);
  attribute_pos.bind(binding_pos);
  attribute_pos.enable();

  // 纹理坐标属性
  auto &attribute_tex = vao.get_attribute(1);
  attribute_tex.set_format(2, GL_FLOAT, GL_FALSE, 0);
  attribute_tex.bind(binding_tex);
  attribute_tex.enable();

  vao.set_element_buffer(ebo);

  // red data 10*10
  std::vector<glm::u8vec4> image_data(100);
  for (auto &pixel : image_data) {
    pixel = glm::u8vec4(255, 0, 0, 255); // Red color
  }

  Texture texture(GL_TEXTURE_2D);
  texture.set_storage_2d(1, GL_RGBA8, 10, 10);
  texture.set_sub_image_2d(0, 0, 0, 10, 10, GL_RGBA, GL_UNSIGNED_BYTE,
                           image_data.data());

  Sampler sampler;
  sampler.set(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  sampler.set(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  sampler.set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  sampler.set(GL_TEXTURE_WRAP_T, GL_REPEAT);

  while (!window->shouldClose()) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    window->pollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // vao bind
    vao.bind();

    // Draw a rectangle
    program.use();

    texture.bind(0);
    sampler.bind(0);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, 0);

    // Swap the screen buffers
    window->swapBuffers();
  }

  // Terminates GLFW, clearing any resources allocated by GLFW.
  window->destroy();

  return 0;
}