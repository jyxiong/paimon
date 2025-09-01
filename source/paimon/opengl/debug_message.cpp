#include "paimon/opengl/debug_message.h"

#include "glad/gl.h"
#include "paimon/core/base/macro.h"
#include <string>

using namespace paimon;

DebugMessage::Callback DebugMessage::s_callback =
    [](const DebugMessage &message) {
      LOG_ERROR("OpenGL Debug Message: [{}] (source: {}, type: {}, id: {}) {}",
                message.severityString(), message.sourceString(),
                message.typeString(), message.m_id, message.m_message);
    };

DebugMessage::DebugMessage(const GLenum source, const GLenum type,
                           const GLuint id, const GLenum severity,
                           const std::string &message)
    : m_source(source), m_type(type), m_id(id), m_severity(severity),
      m_message(message), m_callback(s_callback) {
  glDebugMessageCallback(debugMessageCallback, reinterpret_cast<void*>(this));
}

void DebugMessage::enable() { glEnable(GL_DEBUG_OUTPUT); }

void DebugMessage::disable() { glDisable(GL_DEBUG_OUTPUT); }

bool DebugMessage::isEnabled() { return glIsEnabled(GL_DEBUG_OUTPUT); }

void DebugMessage::setSynchronous(bool synchronous) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

bool DebugMessage::isSynchronous() {
  return glIsEnabled(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void DebugMessage::setCallback(Callback callback) {
  m_callback = callback;
}

void DebugMessage::insert(const DebugMessage &message) {
  glDebugMessageInsert(message.m_source, message.m_type, message.m_id,
                       message.m_severity, message.m_message.size(),
                       message.m_message.c_str());
}

void DebugMessage::control(GLenum source, GLenum type, GLenum severity,
                           GLsizei count, const GLuint *ids,
                           GLboolean enabled) {
  glDebugMessageControl(source, type, severity, count, ids, enabled);
}

std::string DebugMessage::sourceString() const {
  switch (m_source) {
  case GL_DEBUG_SOURCE_API:
    return "API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "Window System";
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "Shader Compiler";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "Third Party";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "Application";
  case GL_DEBUG_SOURCE_OTHER:
  default:
    return "Other";
  }
}

std::string DebugMessage::typeString() const {
  switch (m_type) {
  case GL_DEBUG_TYPE_ERROR:
    return "error";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "deprecated behavior";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "undefined behavior";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "portability";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "performance";
  case GL_DEBUG_TYPE_MARKER:
    return "marker";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "push group";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "pop group";
  case GL_DEBUG_TYPE_OTHER:
  default:
    return "other";
  }
}

std::string DebugMessage::idString() const { return std::to_string(m_id); }

std::string DebugMessage::severityString() const {
  switch (m_severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    return "high";
  case GL_DEBUG_SEVERITY_MEDIUM:
    return "medium";
  case GL_DEBUG_SEVERITY_LOW:
    return "low";
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    return "notification";
  default:
    return "unknown";
  }
}

std::string DebugMessage::message() const { return m_message; }

void GLAPIENTRY DebugMessage::debugMessageCallback(GLenum source, GLenum type,
                                                   GLuint id, GLenum severity,
                                                   GLsizei length,
                                                   const GLchar *message,
                                                   const void *userParam) {
  DebugMessage msg(source, type, id, severity, std::string(message, length));

  const auto *debugMessage = reinterpret_cast<const DebugMessage*>(userParam);
  debugMessage->m_callback(msg);
}