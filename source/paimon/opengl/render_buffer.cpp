#include "paimon/opengl/render_buffer.h"
#include "glad/gl.h"

using namespace paimon;

Renderbuffer::Renderbuffer() : NamedObject(GL_RENDERBUFFER) {
  glCreateRenderbuffers(1, &m_name);
}

Renderbuffer::~Renderbuffer() {
  if (m_name != 0) {
    glDeleteRenderbuffers(1, &m_name);
  }
}

bool Renderbuffer::is_valid() const { return glIsRenderbuffer(m_name); }

void Renderbuffer::bind() const { glBindRenderbuffer(GL_RENDERBUFFER, m_name); }

void Renderbuffer::storage(GLenum internalformat, GLsizei width,
                           GLsizei height) {
  glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
}

void Renderbuffer::storage_multisample(GLsizei samples, GLenum internalformat,
                                       GLsizei width, GLsizei height) {
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat,
                                   width, height);
}

void Renderbuffer::get(GLint value) const {
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &value);
}

GLint Renderbuffer::get() const {
  GLint value;
  get(value);
  return value;
}