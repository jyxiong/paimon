#include "paimon/opengl/error.h"

using namespace paimon;

Error::Error(GLenum errorCode) : m_errorCode(errorCode) {}

Error::Error() : Error(GL_NO_ERROR) {}

GLenum Error::code() const { return m_errorCode; }

std::string Error::code_string() const {
  switch (m_errorCode) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  default:
    return "Unknown GLenum.";
  }
}

bool Error::isError() const { return m_errorCode != GL_NO_ERROR; }

Error::operator bool() const { return isError(); }

Error Error::get() { return Error(glGetError()); }

void Error::clear() {
  while (Error::get())
    ;
}
