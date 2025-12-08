#include "paimon/opengl/vertex_array.h"

#include <glad/gl.h>

using namespace paimon;

VertexArray::VertexArray() : NamedObject(GL_VERTEX_ARRAY) {
  glCreateVertexArrays(1, &m_name);
}

VertexArray::~VertexArray() {
  if (m_name != 0) {
    glDeleteVertexArrays(1, &m_name);
  }
}

bool VertexArray::is_valid() const {
  return glIsVertexArray(m_name) == GL_TRUE;
}

void VertexArray::enable_attribute(GLuint index) const {
  glEnableVertexArrayAttrib(m_name, index);
}

void VertexArray::disable_attribute(GLuint index) const {
  glDisableVertexArrayAttrib(m_name, index);
}

void VertexArray::set_attribute_format(GLuint attribute, GLint size,
                                        GLenum type, GLboolean normalized,
                                        GLuint relative_offset) const {
  glVertexArrayAttribFormat(m_name, attribute, size, type, normalized,
                            relative_offset);
}

void VertexArray::set_binding_divisor(GLuint binding, GLuint divisor) const {
  glVertexArrayBindingDivisor(m_name, binding, divisor);
}

void VertexArray::set_vertex_buffer(uint32_t binding, const Buffer &buffer,
                                    GLintptr offset, GLsizei stride) const {
  glVertexArrayVertexBuffer(m_name, binding, buffer.get_name(), offset, stride);
}

void VertexArray::set_element_buffer(const Buffer &buffer) const {
  glVertexArrayElementBuffer(m_name, buffer.get_name());
}

void VertexArray::set_attribute_binding(GLuint attribute, GLuint binding) const {
  glVertexArrayAttribBinding(m_name, attribute, binding);
}

void VertexArray::bind() const { glBindVertexArray(m_name); }

void VertexArray::unbind() { glBindVertexArray(0); }