#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
class Framebuffer : NamedObject {
public:
  Framebuffer();
  ~Framebuffer();

  Framebuffer(const Framebuffer &other) = delete;
  Framebuffer &operator=(const Framebuffer &other) = delete;

  Framebuffer(Framebuffer &&other) = default;

  bool is_valid() const;

public:
  void bind() const;
  static void unbind();

  void setTexture(GLenum attachment, GLuint texture, GLint level) const;

  void setRenderbuffer(GLenum attachment, GLuint renderbuffer) const;

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
};
} // namespace paimon