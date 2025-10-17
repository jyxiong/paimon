#include <glm/glm.hpp>

#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/platform/window.h"

using namespace paimon;

namespace {
// hard code triangle vertices in shader for simplicity
std::string vertex_source = R"(
  #version 460 core

  layout(location = 0) in vec3 a_position;

  void main()
  {
    gl_Position = vec4(a_position, 1.0f);
  }
  )";

std::string fragment_source = R"(
  #version 460 core
  out vec4 FragColor;
  void main() 
  {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
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

  // VAO setup
  // Define the vertices of a triangle
  float vertices[] = {
      // positions
      -0.5f, -0.5f, 0.0f, // bottom left
      0.5f,  -0.5f, 0.0f, // bottom right
      0.0f,  0.5f,  0.0f  // top
  };
  // Create a Vertex Buffer Object (VBO) and copy the vertices to it
  Buffer vbo;
  vbo.set_storage(sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
  vbo.bind(GL_ARRAY_BUFFER);

  VertexArray vao;
  vao.bind();

  auto &binding = vao.get_binding(0);
  binding.bind_buffer(vbo, 0, 3 * sizeof(float));

  auto &attribute = vao.get_attribute(0);
  attribute.set_format(3, GL_FLOAT, GL_FALSE, 0);
  attribute.bind(binding);
  attribute.enable();

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

    // Draw a triangle
    program.use();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap the screen buffers
    window->swapBuffers();
  }

  return 0;
}