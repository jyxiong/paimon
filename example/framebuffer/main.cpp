#include "paimon/core/log/log_system.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "paimon/opengl/framebuffer.h"
#include "paimon/opengl/texture.h"

#include "screen_quad.h"


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

glm::ivec2 g_size = {800, 600};
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
      glfwCreateWindow(640, 480, "paimon Texture", nullptr, nullptr);
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
      0.5f, -0.5f, 0.0f,  // bottom right
      0.0f, 0.5f, 0.0f    // top
  };
  // Create a Vertex Buffer Object (VBO) and copy the vertices to it
  Buffer vbo;
  vbo.set_storage(sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
  vbo.bind(GL_ARRAY_BUFFER);

  VertexArray vao;
  auto &binding = vao.get_binding(0);
  binding.bind_buffer(vbo, 0, 3 * sizeof(float));

  auto &attribute = vao.get_attribute(0);
  attribute.set_format(3, GL_FLOAT, GL_FALSE, 0);
  attribute.bind(binding);
  attribute.enable();

  Texture texture(GL_TEXTURE_2D);
  texture.set_storage_2d(1, GL_RGBA8, 100, 100);

  Framebuffer fbo;
  fbo.attachTexture(GL_COLOR_ATTACHMENT0, &texture, 0);
  if (!fbo.isComplete(GL_DRAW_FRAMEBUFFER)) {
    LOG_ERROR("Framebuffer is not complete!");
  }

  ScreenQuad quad;

  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    fbo.bind();

    glViewport(0, 0, 100, 100);
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    // vao bind
    vao.bind();
    program.use();
    // Draw a triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    fbo.unbind();

    glViewport(0, 0, g_size.x, g_size.y);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    quad.draw(texture);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Terminates GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();

  return 0;
}