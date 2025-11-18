#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "paimon/app/application.h"
#include "paimon/app/layer.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/vertex_array.h"

using namespace paimon;

namespace {
// hard code triangle vertices in shader for simplicity
const std::string vertex_source = R"(
  #version 460 core

  layout(location = 0) in vec3 a_position;

  void main()
  {
    gl_Position = vec4(a_position, 1.0f);
  }
  )";

const std::string fragment_source = R"(
  #version 460 core
  out vec4 FragColor;
  void main()
  {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
  )";
} // namespace

class TriangleLayer : public Layer {
public:
  TriangleLayer() : Layer("TriangleLayer") {}

  void onAttach() override {
    // Compile shaders
    m_vertexShader.compile(vertex_source);
    m_fragmentShader.compile(fragment_source);

    // Link program
    m_program.attach(m_vertexShader);
    m_program.attach(m_fragmentShader);
    m_program.link();

    // Define the vertices of a triangle
    float vertices[] = {
        // positions
        -0.5f, -0.5f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f, // bottom right
        0.0f,  0.5f,  0.0f  // top
    };

    // Create VBO
    m_vbo.set_storage(sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
    m_vbo.bind(GL_ARRAY_BUFFER);

    // Setup VAO
    m_vao.bind();

    auto &binding = m_vao.get_binding(0);
    binding.bind_buffer(m_vbo, 0, 3 * sizeof(float));

    auto &attribute = m_vao.get_attribute(0);
    attribute.set_format(3, GL_FLOAT, GL_FALSE, 0);
    attribute.bind(binding);
    attribute.enable();
  }

  void onUpdate() override {
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind VAO and program
    m_vao.bind();
    m_program.use();

    // Draw a triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  void onDetach() override {
    // Cleanup resources (RAII handles most cleanup automatically)
  }

  void onEvent(Event &event) override {
    LOG_INFO("{}", event.toString());
  }

private:
  Shader m_vertexShader{GL_VERTEX_SHADER};
  Shader m_fragmentShader{GL_FRAGMENT_SHADER};
  Program m_program;
  Buffer m_vbo;
  VertexArray m_vao;
};

int main() {
  LogSystem::init();
  auto& app = Application::getInstance();
  app.pushLayer(std::make_unique<TriangleLayer>());
  app.run();
  return 0;
}