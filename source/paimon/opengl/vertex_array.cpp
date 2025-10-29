#include "paimon/opengl/vertex_array.h"

#include "glad/gl.h"

#include "paimon/opengl/base/get.h"

using namespace paimon;

VertexArray::Binding::Binding(const VertexArray &vao, GLuint index)
    : m_vao(vao), m_index(index) {}

GLuint VertexArray::Binding::get_index() const {
  return m_index;
}

void VertexArray::Binding::bind_buffer(const Buffer& buffer, GLintptr offset, GLsizei stride) const {
  m_vao.set_vertex_buffer(m_index, buffer, offset, stride);
}

void VertexArray::Binding::set_divisor(GLuint divisor) const {
  m_vao.set_binding_divisor(m_index, divisor);
}

VertexArray::Attribute::Attribute(const VertexArray &vao, GLuint index)
    : m_vao(vao), m_index(index) {}

GLuint VertexArray::Attribute::get_index() const {
  return m_index;
}

void VertexArray::Attribute::enable() const {
  m_vao.enable_attribute(m_index);
}

void VertexArray::Attribute::disable() const {
  m_vao.disable_attribute(m_index);
}

void VertexArray::Attribute::set_format(GLint size, GLenum type, GLboolean normalized, GLuint relative_offset) const {
  m_vao.set_attribute_format(m_index, size, type, normalized, relative_offset);
}

void VertexArray::Attribute::bind(const Binding &binding) const {
  m_vao.set_attribute_binding(m_index, binding.get_index());
}

VertexArray::VertexArray() : NamedObject(GL_VERTEX_ARRAY) {
}

VertexArray::~VertexArray() {
}

void VertexArray::create() {
  if (m_name == 0) {
    glCreateVertexArrays(1, &m_name);
  }
}

void VertexArray::destroy() {
  if (m_name != 0) {
    glDeleteVertexArrays(1, &m_name);
    m_name = 0;
  }
}

bool VertexArray::is_valid() const {
  return glIsVertexArray(m_name) == GL_TRUE;
}

void VertexArray::set_vertex_buffer(GLuint binding_index, const Buffer &buffer,
                                   GLintptr offset, GLsizei stride) const {
  glVertexArrayVertexBuffer(m_name, binding_index, buffer.get_name(), offset,
                            stride);
}

void VertexArray::set_binding_divisor(GLuint binding_index,
                                     GLuint divisor) const {
  glVertexArrayBindingDivisor(m_name, binding_index, divisor);
}

void VertexArray::enable_attribute(GLuint attribute_index) const {
  glEnableVertexArrayAttrib(m_name, attribute_index);
}

void VertexArray::disable_attribute(GLuint attribute_index) const {
  glDisableVertexArrayAttrib(m_name, attribute_index);
}

void VertexArray::set_attribute_format(GLuint attribute_index, GLint size, GLenum type,
                                      GLboolean normalized,
                                      GLuint relative_offset) const {
  glVertexArrayAttribFormat(m_name, attribute_index, size, type, normalized,
                            relative_offset);
}

void VertexArray::set_attribute_binding(GLuint attribute_index,
                                       GLuint binding_index) const {
  glVertexArrayAttribBinding(m_name, attribute_index, binding_index);
}

void VertexArray::set_element_buffer(const Buffer &buffer) const {
  glVertexArrayElementBuffer(m_name, buffer.get_name());
}

VertexArray::Binding &VertexArray::get_binding(GLuint index) {
  auto [it, _] = m_bindings.try_emplace(index, Binding(*this, index));
  return it->second;
}

VertexArray::Attribute &VertexArray::get_attribute(GLuint index) {
  auto [it, _] = m_attributes.try_emplace(index, Attribute(*this, index));
  return it->second;
}

void VertexArray::bind() const { glBindVertexArray(m_name); }

void VertexArray::unbind() { glBindVertexArray(0); }