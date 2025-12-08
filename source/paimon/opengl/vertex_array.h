#pragma once

#include <vector>

#include "paimon/opengl/base/object.h"
#include "paimon/opengl/buffer.h"

namespace paimon {

class VertexArray : public NamedObject {
public:
  VertexArray();
  ~VertexArray() override;

  VertexArray(const VertexArray &other) = delete;
  VertexArray &operator=(const VertexArray &other) = delete;

  VertexArray(VertexArray &&other) = default;

  bool is_valid() const override;

  // attribute methods
  void enable_attribute(GLuint index) const;
  void disable_attribute(GLuint index) const;

  void set_attribute_format(GLuint attribute, GLint size, GLenum type,
                            GLboolean normalized,
                            GLuint relative_offset) const;

  void set_binding_divisor(GLuint binding, GLuint divisor) const;

  void set_vertex_buffer(uint32_t binding, const Buffer &buffer,
                         GLintptr offset, GLsizei stride) const;

  void set_element_buffer(const Buffer &buffer) const;

  void set_attribute_binding(GLuint attribute, GLuint binding) const;

  void bind() const;
  static void unbind();

};

} // namespace paimon