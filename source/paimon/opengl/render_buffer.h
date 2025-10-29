#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
class Renderbuffer : public NamedObject {
public:
  Renderbuffer();
  ~Renderbuffer() override;

  Renderbuffer(const Renderbuffer &other) = delete;
  Renderbuffer &operator=(const Renderbuffer &other) = delete;

  Renderbuffer(Renderbuffer &&other) = default;

  void create();

  void destroy();

  bool is_valid() const override;

public:

  void bind() const;

  void storage(GLenum internalformat, GLsizei width, GLsizei height);

  void storage_multisample(GLsizei samples, GLenum internalformat,
                           GLsizei width, GLsizei height);

  void get(GLint value) const;

  GLint get() const;
};
} // namespace paimon