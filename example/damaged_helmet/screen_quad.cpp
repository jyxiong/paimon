#include "screen_quad.h"

#include "glad/gl.h"

ScreenQuad::ScreenQuad() {
  // Create a minimal VAO (no vertex data needed, vertices are in shader)
  m_vao = std::make_unique<VertexArray>();

  // Setup sampler for texture filtering
  m_sampler = std::make_unique<Sampler>();
  m_sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_sampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void ScreenQuad::setProgram(Program *program) { m_program = program; }

void ScreenQuad::draw(const Texture &texture) {
  m_vao->bind();
  if (m_program) {
    m_program->use();
  }
  texture.bind(6);
  m_sampler->bind(6);
  // Draw a single triangle that covers the entire screen
  glDrawArrays(GL_TRIANGLES, 0, 3);
}
