#pragma once

#include <unordered_map>

#include "paimon/opengl/base/object.h"
#include "paimon/opengl/buffer.h"

namespace paimon {

class VertexArray : public NamedObject {
public:
  class Binding {
  public:
    Binding(const VertexArray &vao, GLuint index);

    GLuint get_index() const;

    void
    bind_buffer(const Buffer &buffer, GLintptr offset, GLsizei stride) const;

    void set_divisor(GLuint divisor) const;

  private:
    const VertexArray &m_vao;
    GLuint m_index;
  };

  class Attribute {
  public:
    Attribute(const VertexArray &vao, GLuint index);

    GLuint get_index() const;

    void enable() const;

    void disable() const;

    void set_format(
      GLint size, GLenum type, GLboolean normalized, GLuint relative_offset
    ) const;

    void bind(const Binding &binding) const;

  private:
    const VertexArray &m_vao;
    GLuint m_index;
  };

public:
  VertexArray();
  ~VertexArray() override;

  VertexArray(const VertexArray &other) = delete;
  VertexArray &operator=(const VertexArray &other) = delete;

  void create();

  void destroy();

  bool is_valid() const override;

public:
  void set_vertex_buffer(
    GLuint binding_index, const Buffer &buffer, GLintptr offset, GLsizei stride
  ) const;

  void set_binding_divisor(GLuint binding_index, GLuint divisor) const;

  void enable_attribute(GLuint attribute_index) const;
  void disable_attribute(GLuint attribute_index) const;

  void set_attribute_format(
    GLuint attribute_index, GLint size, GLenum type, GLboolean normalized,
    GLuint relative_offset
  ) const;

  void set_attribute_binding(GLuint attribute_index, GLuint binding_index) const;

  void set_element_buffer(const Buffer &buffer) const;

  Binding &get_binding(GLuint index);

  Attribute &get_attribute(GLuint index);

  void bind() const;
  static void unbind();

private:
  std::unordered_map<GLuint, Binding> m_bindings;
  std::unordered_map<GLuint, Attribute> m_attributes;
};

} // namespace paimon