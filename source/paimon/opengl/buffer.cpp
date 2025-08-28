#include "paimon/opengl/buffer.h"

using namespace paimon;

Buffer::Buffer()
    : NamedObject(GL_BUFFER) {
  glCreateBuffers(1, &m_name);
}

Buffer::~Buffer() {
  if (m_name != 0) {
    glDeleteBuffers(1, &m_name);
  }
}

bool Buffer::is_valid() const {
  return glIsBuffer(m_name) == GL_TRUE;
}

void Buffer::set_storage(GLsizeiptr size, const void *data, GLbitfield flags) const {
  glNamedBufferStorage(m_name, size, data, flags);
}

void Buffer::set_data(GLintptr offset, GLsizeiptr size, const void *data) const {
  glNamedBufferData(m_name, size, data, GL_DYNAMIC_DRAW);
}

void Buffer::set_sub_data(GLintptr offset, GLsizeiptr size, const void *data) const {
  glNamedBufferSubData(m_name, offset, size, data);
}

void Buffer::invalidate_data() const {
  glInvalidateBufferData(m_name);
}

void Buffer::invalidate_sub_data(GLintptr offset, GLsizeiptr size) const {
  glInvalidateBufferSubData(m_name, offset, size);
}

void Buffer::copy_sub_data(const Buffer &source, GLintptr read_offset, GLintptr write_offset, GLsizeiptr size) const {
  glCopyNamedBufferSubData(source.m_name, m_name, read_offset, write_offset, size);
}

void Buffer::clear_data(GLenum internalformat, GLenum format, GLenum type, const void *data) const {
  glClearNamedBufferData(m_name, internalformat, format, type, data);
}

void Buffer::map(GLenum access) const {
  glMapNamedBuffer(m_name, access);
}

void *Buffer::map_range(GLintptr offset, GLsizeiptr length, GLbitfield access) const {
  return glMapNamedBufferRange(m_name, offset, length, access);
}

void Buffer::flush_mapped_range(GLintptr offset, GLsizeiptr length) const {
  glFlushMappedNamedBufferRange(m_name, offset, length);
}

bool Buffer::unmap() const {
  return glUnmapNamedBuffer(m_name) == GL_TRUE;
}

void Buffer::bind(GLenum target) const {
  glBindBuffer(target, m_name);
}

void Buffer::unbind(GLenum target) {
  glBindBuffer(target, 0);
}

void Buffer::bind_base(GLenum target, GLuint index) const {
  glBindBufferBase(target, index, m_name);
}

void Buffer::bind_range(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size) const {
  glBindBufferRange(target, index, m_name, offset, size);
}

template<>
void Buffer::get<GLint>(GLenum pname, GLint *params) const {
  glGetNamedBufferParameteriv(m_name, pname, params);
}

template<>
void Buffer::get<GLint64>(GLenum pname, GLint64 *params) const {
  glGetNamedBufferParameteri64v(m_name, pname, params);
}

void* Buffer::get_pointer() const {
  void* ptr = nullptr;
  glGetNamedBufferPointerv(m_name, GL_BUFFER_MAP_POINTER, &ptr);
  return ptr;
}

void Buffer::get_sub_data(GLintptr offset, GLsizeiptr size, void *data) const {
  glGetNamedBufferSubData(m_name, offset, size, data);
}
