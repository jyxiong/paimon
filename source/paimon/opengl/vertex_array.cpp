#include "paimon/opengl/vertex_array.h"
#include "glad/gl.h"

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

void VertexArray::bind() const { glBindVertexArray(m_name); }

void VertexArray::unbind() { glBindVertexArray(0); }