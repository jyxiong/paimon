#include "paimon/opengl/vertex_array.h"

#include "glad/gl.h"

#include "paimon/opengl/base/get.h"

using namespace paimon;

VertexArray::Binding::Binding(const VertexArray &vao, GLuint index)
    : m_vao(vao), m_index(index) {}

GLuint VertexArray::Binding::get_index() const { return m_index; }

void VertexArray::Binding::bind_buffer(const Buffer &buffer, GLintptr offset,
                                       GLsizei stride) const {
  glVertexArrayVertexBuffer(m_vao.m_name, m_index, buffer.get_name(), offset,
                            stride);
}

void VertexArray::Binding::set_divisor(GLuint divisor) const {
  glVertexArrayBindingDivisor(m_vao.m_name, m_index, divisor);
}

VertexArray::Attribute::Attribute(const VertexArray &vao, GLuint index)
    : m_vao(vao), m_index(index) {}

GLuint VertexArray::Attribute::get_index() const { return m_index; }

void VertexArray::Attribute::enable() const {
  glEnableVertexArrayAttrib(m_vao.m_name, m_index);
}

void VertexArray::Attribute::disable() const {
  glDisableVertexArrayAttrib(m_vao.m_name, m_index);
}

void VertexArray::Attribute::set_format(GLint size, GLenum type,
                                        GLboolean normalized,
                                        GLuint relative_offset) const {
  glVertexArrayAttribFormat(m_vao.m_name, m_index, size, type, normalized,
                            relative_offset);
}

void VertexArray::Attribute::bind(const Binding &binding) const {
  glVertexArrayAttribBinding(m_vao.m_name, m_index, binding.get_index());
}

VertexArray::VertexArray() : NamedObject(GL_VERTEX_ARRAY) {
  glCreateVertexArrays(1, &m_name);

  // TODO: get max counts from OpenGL context
  auto max_binding_count = get<GLint>(GL_MAX_VERTEX_ATTRIB_BINDINGS);
  m_bindings.reserve(max_binding_count);
  for (GLuint i = 0; i < static_cast<GLuint>(max_binding_count); ++i) {
    m_bindings.emplace_back(*this, i);
  }

  // TODO: get max counts from OpenGL context
  auto max_attribute_count = get<GLint>(GL_MAX_VERTEX_ATTRIBS);
  m_attributes.reserve(max_attribute_count);
  for (GLuint i = 0; i < static_cast<GLuint>(max_attribute_count); ++i) {
    m_attributes.emplace_back(*this, i);
  }
}

VertexArray::~VertexArray() {
  if (m_name != 0) {
    glDeleteVertexArrays(1, &m_name);
  }
}

bool VertexArray::is_valid() const {
  return glIsVertexArray(m_name) == GL_TRUE;
}

void VertexArray::set_element_buffer(const Buffer &buffer) const {
  glVertexArrayElementBuffer(m_name, buffer.get_name());
}

VertexArray::Binding &VertexArray::get_binding(GLuint index) {
  // TODO: assert index in range
  return m_bindings[index];
}

VertexArray::Attribute &VertexArray::get_attribute(GLuint index) {
  // TODO: assert index in range
  return m_attributes[index];
}

void VertexArray::bind() const { glBindVertexArray(m_name); }

void VertexArray::unbind() { glBindVertexArray(0); }