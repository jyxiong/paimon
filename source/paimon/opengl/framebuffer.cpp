#include "paimon/opengl/framebuffer.h"

using namespace paimon;

Framebuffer::Framebuffer() : NamedObject(GL_FRAMEBUFFER) {}

Framebuffer::~Framebuffer() {
  if (m_name != 0) {
    glDeleteFramebuffers(1, &m_name);
  }
}

bool Framebuffer::is_valid() const { return glIsFramebuffer(m_name); }

void Framebuffer::bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_name); }

void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::bindDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::setTexture(GLenum attachment, GLenum textarget, GLuint texture,
                             GLint level) const {
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, texture, level);
}

void Framebuffer::setRenderbuffer(GLenum attachment,
                                  GLuint renderbuffer) const {
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER,
                            renderbuffer);
}

void Framebuffer::setDrawBuffers(GLsizei n, const GLenum *bufs) const {
  glDrawBuffers(n, bufs);
}

void Framebuffer::setReadBuffer(GLenum src) const {
  glReadBuffer(src);
}

void Framebuffer::invalidate(GLsizei n, const GLenum *attachments) const {
  glInvalidateFramebuffer(GL_FRAMEBUFFER, n, attachments);
}

void Framebuffer::invalidateSub(GLsizei n, const GLenum *attachments, GLint x,
                                 GLint y, GLsizei width,
                                 GLsizei height) const {
  glInvalidateSubFramebuffer(GL_FRAMEBUFFER, n, attachments, x, y, width,
                             height);
}

bool Framebuffer::isComplete() const {
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::set(GLenum param, GLint value) const {
  glFramebufferParameteri(GL_FRAMEBUFFER, param, value);
}

template<>
void Framebuffer::clear(GLenum buffer, GLint draw_buffer, const GLfloat* value) {
  glClearNamedFramebufferfv(m_name, buffer, draw_buffer, value);
}

template<>
void Framebuffer::clear(GLenum buffer, GLint draw_buffer, const GLint* value) {
  glClearNamedFramebufferiv(m_name, buffer, draw_buffer, value);
}

template<>
void Framebuffer::clear(GLenum buffer, GLint draw_buffer, const GLuint* value) {
  glClearNamedFramebufferuiv(m_name, buffer, draw_buffer, value);
}

void Framebuffer::clear(GLenum buffer, GLint draw_buffer, GLfloat depth,
                        GLint stencil) {
  glClearNamedFramebufferfi(m_name, buffer, draw_buffer, depth, stencil);
}
