#include "paimon/opengl/debug_message.h"

using namespace paimon;

DebugMessage::DebugMessage(const GLenum source, const GLenum type,
                           const GLuint id, const GLenum severity,
                           const std::string &message)
    : m_source(source), m_type(type), m_id(id), m_severity(severity),
      m_message(message) {}

void DebugMessage::enable() { glEnable(GL_DEBUG_OUTPUT); }

void DebugMessage::disable() { glDisable(GL_DEBUG_OUTPUT); }

bool DebugMessage::isEnabled() { glIsEnabled(GL_DEBUG_OUTPUT); }

void DebugMessage::setSynchronous(bool synchronous) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

bool DebugMessage::isSynchronous() { glIsEnabled(GL_DEBUG_OUTPUT_SYNCHRONOUS); }

void DebugMessage::setCallback(Callback callback) {
  s_callback = callback;

  glDebugMessageCallback(
      [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
         const GLchar *message, const void *userParam) {
        if (s_callback) {
          DebugMessage debugMessage(source, type, id, severity,
                                    std::string(message, length));
          s_callback(debugMessage);
        }
      },
      nullptr);
}

void DebugMessage::insertMessage(const DebugMessage &message) {
  glDebugMessageInsert(message.m_source, message.m_type, message.m_id,
                       message.m_severity, message.m_message.size(),
                       message.m_message.c_str());
}

void DebugMessage::controlMessages(GLenum source, GLenum type, GLenum severity,
                                   GLsizei count, const GLuint *ids,
                                   GLboolean enabled) {
  glDebugMessageControl(source, type, severity, count, ids, enabled);
}