#pragma once

#include <vector>

#include "paimon/opengl/base/object.h"
#include "paimon/opengl/buffer.h"

namespace paimon {

  class VertexArray : public NamedObject {
public:
  class Binding {
  public:
    Binding(const VertexArray &vao, GLuint index);

    GLuint get_index() const;

    void bind_buffer(const Buffer& buffer, GLintptr offset, GLsizei stride) const;

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

    void set_format(GLint size, GLenum type, GLboolean normalized, GLuint relative_offset) const;

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

  VertexArray(VertexArray &&other) = default;

  bool is_valid() const override;

  void set_element_buffer(const Buffer &buffer) const;

  Binding &get_binding(GLuint index);

  Attribute &get_attribute(GLuint index);

  void bind() const;
  static void unbind();

private:
  std::vector<Binding> m_bindings;
  std::vector<Attribute> m_attributes;
};

} // namespace paimon