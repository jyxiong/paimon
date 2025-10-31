#include "paimon/opengl/framebuffer.h"
#include "framebuffer.h"
#include "paimon/opengl/texture.h"
#include "render_buffer.h"

using namespace paimon;

FramebufferAttachment::FramebufferAttachment(Framebuffer *framebuffer,
                                             GLenum attachment)
    : m_framebuffer(framebuffer), m_attachment(attachment) {}

GLenum FramebufferAttachment::getAttachment() const { return m_attachment; }

RenderbufferAttachment::RenderbufferAttachment(Framebuffer *framebuffer,
                                               GLenum attachment,
                                               Renderbuffer *renderbuffer)
    : FramebufferAttachment(framebuffer, attachment),
      m_renderbuffer(renderbuffer) {}

TextureAttachment::TextureAttachment(Framebuffer *framebuffer,
                                     GLenum attachment, Texture *texture,
                                     GLint level, GLint layer)
    : FramebufferAttachment(framebuffer, attachment),
      m_texture(texture->get_name()), m_level(level), m_layer(layer) {}

Framebuffer::Framebuffer() : NamedObject(GL_FRAMEBUFFER) {
  glCreateFramebuffers(1, &m_name);
}

Framebuffer::~Framebuffer() {
  if (m_name != 0) {
    glDeleteFramebuffers(1, &m_name);
  }
}

bool Framebuffer::is_valid() const { return glIsFramebuffer(m_name); }

void Framebuffer::bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_name); }

void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::attachTexture(GLenum attachment, Texture *texture,
                                GLint level) {
  glNamedFramebufferTexture(m_name, attachment, texture->get_name(), level);

  addAttachment(
      std::make_unique<TextureAttachment>(this, attachment, texture, level));
}

void Framebuffer::attachTextureLayer(GLenum attachment, Texture *texture,
                                     GLint level, GLint layer) {
  glNamedFramebufferTextureLayer(m_name, attachment, texture->get_name(), level,
                                 layer);

  addAttachment(std::make_unique<TextureAttachment>(this, attachment, texture,
                                                    level, layer));
}

void Framebuffer::attachRenderbuffer(GLenum attachment,
                                     Renderbuffer *renderbuffer) {
  glNamedFramebufferRenderbuffer(m_name, attachment, GL_RENDERBUFFER,
                                 renderbuffer->get_name());

  addAttachment(
      std::make_unique<RenderbufferAttachment>(this, attachment, renderbuffer));
}

void Framebuffer::setDrawBuffers(GLsizei n, const GLenum *bufs) const {
  glNamedFramebufferDrawBuffers(m_name, n, bufs);
}

void Framebuffer::setReadBuffer(GLenum src) const {
  glNamedFramebufferDrawBuffer(m_name, src);
}

void Framebuffer::invalidate(GLsizei n, const GLenum *attachments) const {
  glInvalidateNamedFramebufferData(m_name, n, attachments);
}

void Framebuffer::invalidateSub(GLsizei n, const GLenum *attachments, GLint x,
                                GLint y, GLsizei width, GLsizei height) const {
  glInvalidateNamedFramebufferSubData(m_name, n, attachments, x, y, width,
                                      height);
}

bool Framebuffer::isComplete(GLenum target) const {
  return glCheckNamedFramebufferStatus(m_name, target) ==
         GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::set(GLenum param, GLint value) const {
  glNamedFramebufferParameteri(m_name, param, value);
}

template <>
void Framebuffer::clear(GLenum buffer, GLint draw_buffer,
                        const GLfloat *value) {
  glClearNamedFramebufferfv(m_name, buffer, draw_buffer, value);
}

template <>
void Framebuffer::clear(GLenum buffer, GLint draw_buffer, const GLint *value) {
  glClearNamedFramebufferiv(m_name, buffer, draw_buffer, value);
}

template <>
void Framebuffer::clear(GLenum buffer, GLint draw_buffer, const GLuint *value) {
  glClearNamedFramebufferuiv(m_name, buffer, draw_buffer, value);
}

void Framebuffer::clear(GLenum buffer, GLint draw_buffer, GLfloat depth,
                        GLint stencil) {
  glClearNamedFramebufferfi(m_name, buffer, draw_buffer, depth, stencil);
}

void Framebuffer::addAttachment(
    std::unique_ptr<FramebufferAttachment> &&attachment) {
  m_attachments[attachment->getAttachment()] = std::move(attachment);
}
