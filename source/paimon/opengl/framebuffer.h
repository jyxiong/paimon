#pragma once

#include <map>
#include <memory>

#include "paimon/opengl/base/object.h"
#include "paimon/opengl/render_buffer.h"
#include "paimon/opengl/texture.h"

namespace paimon {

class Framebuffer;

class FramebufferAttachment {
public:
  FramebufferAttachment(Framebuffer *framebuffer, GLenum attachment);

  GLenum getAttachment() const;

private:
  Framebuffer *m_framebuffer;
  GLenum m_attachment;
};

class RenderbufferAttachment : public FramebufferAttachment {
public:
  RenderbufferAttachment(Framebuffer *framebuffer, GLenum attachment,
                         Renderbuffer *renderbuffer);

private:
  Renderbuffer *m_renderbuffer;
};

class TextureAttachment : public FramebufferAttachment {
public:
  TextureAttachment(Framebuffer *framebuffer, GLenum attachment,
                    Texture *texture, GLint level, GLint layer = 0);

private:
  GLuint m_texture;
  GLint m_level;
  GLint m_layer;
};

class Framebuffer : NamedObject {
public:
  Framebuffer();
  ~Framebuffer();

  Framebuffer(const Framebuffer &other) = delete;
  Framebuffer &operator=(const Framebuffer &other) = delete;

  Framebuffer(Framebuffer &&other) = default;

  bool is_valid() const override;

public:
  void bind() const;
  static void unbind();

  void attachTexture(GLenum attachment, Texture *texture, GLint level=0);
  void attachTextureLayer(GLenum attachment, Texture *texture, GLint level=0, GLint layer=0);
  void attachRenderbuffer(GLenum attachment, Renderbuffer *renderbuffer);

  void setDrawBuffers(GLsizei n, const GLenum *bufs) const;
  void setReadBuffer(GLenum src) const;

  void invalidate(GLsizei n, const GLenum *attachments) const;
  void invalidateSub(GLsizei n, const GLenum *attachments, GLint x, GLint y,
                     GLsizei width, GLsizei height) const;

  bool isComplete(GLenum target) const;

  void set(GLenum param, GLint value) const;

  template <typename T>
  void clear(GLenum buffer, GLint draw_buffer, const T *value);
  void clear(GLenum buffer, GLint draw_buffer, GLfloat depth, GLint stencil);

private:
  void addAttachment(std::unique_ptr<FramebufferAttachment> &&attachment);

private:
  std::map<GLenum, std::unique_ptr<FramebufferAttachment>> m_attachments;
};
} // namespace paimon