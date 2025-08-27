#include "paimon/opengl/base/object.h"

#include <vector>

#include "paimon/opengl/base/get.h"

using namespace paimon;

NamedObject::NamedObject(GLenum identifier)
    : m_identifier(identifier), m_name(0) {}

NamedObject::NamedObject(NamedObject &&other) noexcept
    : Object(std::move(other)), m_identifier(other.m_identifier),
      m_name(other.m_name) {
  other.m_name = 0;
}

std::string NamedObject::get_label() const {
  GLint maxLabelLen = get<GLint>(GL_MAX_LABEL_LENGTH);
  std::vector<GLchar> buffer(maxLabelLen);
  GLsizei labelLen = 0;
  glGetObjectLabel(m_identifier, m_name, maxLabelLen, &labelLen, buffer.data());
  return std::string(buffer.data(), labelLen);
}

void NamedObject::set_label(const std::string &label) {
  glObjectLabel(m_identifier, m_name, static_cast<GLsizei>(label.size()),
                label.data());
}

GLint NamedObject::get_name() const { return m_name; }

SyncObject::SyncObject(SyncObject &&other) noexcept
    : Object(std::move(other)), m_sync(other.m_sync) {
  other.m_sync = nullptr;
}

std::string SyncObject::get_label() const {
  GLint maxLabelLen = get<GLint>(GL_MAX_LABEL_LENGTH);
  std::vector<GLchar> buffer(maxLabelLen);
  GLsizei labelLen = 0;
  glGetObjectPtrLabel(m_sync, maxLabelLen, &labelLen, buffer.data());
  return std::string(buffer.data(), labelLen);
}

void SyncObject::set_label(const std::string &label) {
  glObjectPtrLabel(m_sync, static_cast<GLsizei>(label.size()), label.data());
}