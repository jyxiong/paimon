#pragma once

#include <functional>
#include <string>

#include "glad/gl.h"

namespace paimon {

class DebugMessage {
public:
  using Callback = std::function<void(const DebugMessage &)>;

  static Callback s_callback;

public:
  DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
               const std::string &message);

  ~DebugMessage() = default;

  std::string sourceString() const;

  std::string typeString() const;

  std::string idString() const;

  std::string severityString() const;

  std::string message() const;

  static void enable();

  static void disable();

  static bool isEnabled();

  static void setSynchronous(bool synchronous);

  static bool isSynchronous();

  void setCallback(Callback callback);

  static void insert(const DebugMessage &message);

  static void control(GLenum source, GLenum type, GLenum severity,
                      GLsizei count, const GLuint *ids, GLboolean enabled);

private:
  static void GLAPIENTRY debugMessageCallback(GLenum source, GLenum type,
                                              GLuint id, GLenum severity,
                                              GLsizei length,
                                              const GLchar *message,
                                              const void *userParam);

protected:
  GLenum m_source;
  GLenum m_type;
  GLuint m_id;
  GLenum m_severity;
  std::string m_message;

  Callback m_callback;
};
} // namespace paimon