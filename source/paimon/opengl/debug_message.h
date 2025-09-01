#pragma once

#include <functional>
#include <string>

#include "glad/gl.h"

namespace paimon {

class DebugMessage {
public:
  using Callback = std::function<void(const DebugMessage &)>;

public:
  DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
               const std::string &message);

  static void enable();

  static void disable();

  static bool isEnabled();

  static void setSynchronous(bool synchronous);

  static bool isSynchronous();

  static void setCallback(Callback callback);

  static void insertMessage(const DebugMessage &message);

  static void controlMessages(GLenum source, GLenum type, GLenum severity,
                              GLsizei count, const GLuint *ids,
                              GLboolean enabled);

protected:
  GLenum m_source;
  GLenum m_type;
  GLuint m_id;
  GLenum m_severity;
  std::string m_message;

  static Callback s_callback;
};
} // namespace paimon