#include "paimon/core/log_system.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "paimon/opengl/program.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/shader_program.h"
#include "paimon/opengl/program_pipeline.h"

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

void error(int errnum, const char *errmsg) {
  LOG_ERROR("GLFW error {}: {}", errnum, errmsg);
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
  g_size = glm::ivec2{width, height};
}

void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action,
                  int /*modes*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    glfwSetWindowShouldClose(window, true);
}

void glfwError(int error_code, const char *description) {
  LOG_INFO("GLFW error {}: {}", error_code, description);
}

int main() {
  LogSystem::init();

  if (!glfwInit()) {
    LOG_ERROR("GLFW initialization failed. Terminate execution.");

    return 1;
  }

  glfwSetErrorCallback(error);

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  // Create a context and, if valid, make it current
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "globjects Texture", nullptr, nullptr);
  if (window == nullptr) {
    LOG_ERROR("GLFW window creation failed. Terminate execution.");

    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    LOG_ERROR("Failed to initialize OpenGL context");

    return -1;
  }

  ShaderProgram vertex_shader(GL_VERTEX_SHADER, vertex_source);
  ShaderProgram fragment_shader(GL_FRAGMENT_SHADER, fragment_source);

  ProgramPipeline pipeline;
  pipeline.use_program_stages(GL_VERTEX_SHADER_BIT, vertex_shader.get_name());
  pipeline.use_program_stages(GL_FRAGMENT_SHADER_BIT,
                              fragment_shader.get_name());
  if (!pipeline.validate()) {
    LOG_ERROR("Program pipeline validation failed");
  }

  // VAO setup
  // Define the vertices of a triangle
  float vertices[] = {
      // positions
      -0.5f, -0.5f, 0.0f, // bottom left
      0.5f, -0.5f, 0.0f,  // bottom right
      0.0f, 0.5f, 0.0f    // top
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

  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // vao bind
    vao.bind();

    // Draw a triangle
    pipeline.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Terminates GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();

  return 0;
}