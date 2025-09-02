#include "paimon/opengl/capability.h"

using namespace paimon;

Capability::Capability(GLenum cap) : m_cap(cap) {
  m_enabled = glIsEnabled(cap) == GL_TRUE;
}

void Capability::enable() {
  if (m_enabled) {
    return; // Already enabled
  }
  m_enabled = true;
  glEnable(m_cap);
}

void Capability::disable() {
  if (!m_enabled) {
    return; // Already disabled
  }
  m_enabled = false;
  glDisable(m_cap);
}

bool Capability::isEnabled() const {
  return m_enabled;
}

void Capability::enable(GLuint index) {
  if (m_enabledi.at(index)) {
    return; // Already enabled
  }
  glEnablei(m_cap, index);
}

void Capability::disable(GLuint index) {
  if (!m_enabledi.at(index)) {
    return; // Already disabled
  }
  glDisablei(m_cap, index);
}

bool Capability::isEnabled(GLuint index) const {
  return m_enabledi.at(index);
}