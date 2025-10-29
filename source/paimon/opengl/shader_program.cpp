#include "paimon/opengl/shader_program.h"

#include <vector>
#include "glad/gl.h"

using namespace paimon;

ShaderProgram::ShaderProgram(GLenum type, const std::string &source) : NamedObject(GL_PROGRAM)
, m_type(type), m_source(source.c_str()) {
}

ShaderProgram::~ShaderProgram() {
}

void ShaderProgram::create() {
  if (m_name == 0) {
    m_name = glCreateShaderProgramv(m_type, 1, &m_source);
  }
}

void ShaderProgram::destroy() {
  if (m_name != 0) {
    glDeleteProgram(m_name);
    m_name = 0;
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

