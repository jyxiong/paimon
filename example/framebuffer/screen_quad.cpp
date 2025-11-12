#include "screen_quad.h"

#include "glad/gl.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/shader.h"

const char *ScreenQuad::s_vertexShaderSource = R"(
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

const char *ScreenQuad::s_fragmentShaderSource = R"(
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

const float ScreenQuad::s_vertices[20] = {
    // positions        // texture coords
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // top left
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f  // top right
};

const unsigned int ScreenQuad::s_indices[6] = {
    0, 1, 2, // First triangle
    1, 3, 2  // Second triangle
};

ScreenQuad::ScreenQuad() {
  m_vertex_buffer = std::make_unique<Buffer>();
  m_vertex_buffer->set_storage(sizeof(s_vertices), s_vertices,
                               GL_DYNAMIC_STORAGE_BIT);
  m_vertex_buffer->bind(GL_ARRAY_BUFFER);

  m_index_buffer = std::make_unique<Buffer>();
  m_index_buffer->set_storage(sizeof(s_indices), s_indices,
                              GL_DYNAMIC_STORAGE_BIT);
  m_index_buffer->bind(GL_ELEMENT_ARRAY_BUFFER);

  m_vao = std::make_unique<VertexArray>();
  auto &binding = m_vao->get_binding(0);
  binding.bind_buffer(*m_vertex_buffer, 0, 5 * sizeof(float));

  // 位置属性
  auto &attribute_pos = m_vao->get_attribute(0);
  attribute_pos.set_format(3, GL_FLOAT, GL_FALSE, 0);
  attribute_pos.bind(binding);
  attribute_pos.enable();

  // 纹理坐标属性
  auto &attribute_tex = m_vao->get_attribute(1);
  attribute_tex.set_format(2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
  attribute_tex.bind(binding);
  attribute_tex.enable();

  m_vao->set_element_buffer(*m_index_buffer);

  Shader vertex_shader(GL_VERTEX_SHADER);
  Shader fragment_shader(GL_FRAGMENT_SHADER);

  if (!vertex_shader.compile(s_vertexShaderSource)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertex_shader.get_info_log());
  }
  if (!fragment_shader.compile(s_fragmentShaderSource)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragment_shader.get_info_log());
  }

  m_program = std::make_unique<Program>();
  m_program->attach(vertex_shader);
  m_program->attach(fragment_shader);
  if (!m_program->link()) {
    LOG_ERROR("Shader program linking failed: {}", m_program->get_info_log());
  }

  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void ScreenQuad::draw(const Texture &texture) {
  m_vao->bind();
  m_program->use();
  texture.bind(0);
  m_sampler->bind(0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}