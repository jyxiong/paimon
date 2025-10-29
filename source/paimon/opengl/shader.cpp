#include "paimon/opengl/shader.h"

#include <vector>

using namespace paimon;

Shader::Shader(GLenum type) : NamedObject(GL_SHADER), m_type(type) {
}

Shader::~Shader() {
}

void Shader::create() {
  if (m_name == 0) {
    m_name = glCreateShader(m_type);
  }
}

void Shader::destroy() {
  if (m_name != 0) {
    glDeleteShader(m_name);
    m_name = 0;
  }
}

bool Shader::is_valid() const { return glIsShader(m_name) == GL_TRUE; }

void Shader::get(GLenum pname, GLint *params) const {
  glGetShaderiv(m_name, pname, params);
}

GLint Shader::get(GLenum pname) const {
  GLint param;
  get(pname, &param);
  return param;
}

std::string Shader::get_info_log() const {
  GLint length = get(GL_INFO_LOG_LENGTH);
  if (length == 0) {
    return std::string();
  }

  std::vector<GLchar> info_log(length);
  glGetShaderInfoLog(m_name, length, 0, info_log.data());
  return std::string(info_log.data(), length);
}

std::string Shader::get_source() const {
  GLint length = get(GL_SHADER_SOURCE_LENGTH);
  if (length == 0) {
    return std::string();
  }

  std::vector<GLchar> source(length);
  glGetShaderSource(m_name, length, 0, source.data());
  return std::string(source.data(), length);
}

GLenum Shader::get_type() const { return m_type; }

bool Shader::compile(const std::string &source) {
  const GLchar *sources = source.c_str();
  glShaderSource(m_name, 1, &sources, 0);
  glCompileShader(m_name);

  return get(GL_COMPILE_STATUS) == GL_TRUE;
}