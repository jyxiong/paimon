#include "paimon/opengl/shader_program.h"

#include <vector>

#include <glad/gl.h>

using namespace paimon;

ShaderProgram::ShaderProgram(GLenum type, const std::string &source)
    : NamedObject(GL_PROGRAM) {
  const GLchar *sources = source.c_str();
  m_name = glCreateShaderProgramv(type, 1, &sources);
}

ShaderProgram::~ShaderProgram() {
  if (m_name != 0) {
    glDeleteProgram(m_name);
  }
}

bool ShaderProgram::is_valid() const {
  return m_name != 0 && glIsProgram(m_name) == GL_TRUE;
}

void ShaderProgram::get(GLenum pname, GLint *params) const {
  glGetProgramiv(m_name, pname, params);
}

GLint ShaderProgram::get(GLenum pname) const {
  GLint param;
  get(pname, &param);
  return param;
}

std::string ShaderProgram::get_info_log() const {
  GLint length = get(GL_INFO_LOG_LENGTH);
  if (length == 0) {
    return std::string();
  }

  std::vector<GLchar> info_log(length);
  glGetProgramInfoLog(m_name, length, 0, info_log.data());
  return std::string(info_log.data(), length);
}
