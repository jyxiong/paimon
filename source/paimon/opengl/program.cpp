#include "paimon/opengl/program.h"

#include <vector>

using namespace paimon;

Program::Program() : NamedObject(GL_PROGRAM) { m_name = glCreateProgram(); }

Program::~Program() {
  if (m_name != 0) {
    glDeleteProgram(m_name);
  }
}

bool Program::is_valid() const { return glIsProgram(m_name) == GL_TRUE; }

void Program::get(GLenum pname, GLint *params) const {
  glGetProgramiv(m_name, pname, params);
}

GLint Program::get(GLenum pname) const {
  GLint param;
  get(pname, &param);
  return param;
}

void Program::set(GLenum pname, GLint param) const {
  // TODO: check pname
  // if pname is GL_PROGRAM_SEPARABLE, must be set to GL_TRUE before
  // glLinkProgram is called if pname is GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
  // recommended to be set to GL_TRUE before glLinkProgram is called
  glProgramParameteri(m_name, pname, param);
}

std::string Program::get_info_log() const {
  GLint length = get(GL_INFO_LOG_LENGTH);
  if (length == 0) {
    return std::string();
  }

  std::vector<GLchar> info_log(length);
  glGetProgramInfoLog(m_name, length, 0, info_log.data());
  return std::string(info_log.data(), length);
}

void Program::attach(const Shader &shader) const {
  glAttachShader(m_name, shader.get_name());
}

void Program::detach(const Shader &shader) const {
  glDetachShader(m_name, shader.get_name());
}

bool Program::link() const {
  glLinkProgram(m_name);
  return get(GL_LINK_STATUS) == GL_TRUE;
}

void Program::use() const { glUseProgram(m_name); }