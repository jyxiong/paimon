#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {

class Sampler : public NamedObject {
public:
  Sampler();
  ~Sampler();

  Sampler(const Sampler &other) = delete;
  Sampler &operator=(const Sampler &other) = delete;

  Sampler(Sampler &&other) = default;

  void create();

  void destroy();

  bool is_valid() const override;

public:
  void bind(GLuint unit);

  template <class T> void get(GLenum property, T *value);

  template <class T> T get(GLenum property);

  template <class T> void set(GLenum property, T value);
};

} // namespace paimon