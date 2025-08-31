#include "paimon/opengl/separate_program.h"
#include "glad/gl.h"

using namespace paimon;

SeparateProgram::SeparateProgram(GLenum type, const std::string &source) : NamedObject(GL_PROGRAM) {
  const GLchar *sources = source.c_str();
  m_name = glCreateShaderProgramv(type, 1, &sources);
}