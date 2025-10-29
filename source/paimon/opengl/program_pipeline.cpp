#include "paimon/opengl/program_pipeline.h"

using namespace paimon;

ProgramPipeline::ProgramPipeline() : NamedObject(GL_PROGRAM_PIPELINE) {
}

ProgramPipeline::~ProgramPipeline() {
}

void ProgramPipeline::create() {
  if (m_name == 0) {
    glCreateProgramPipelines(1, &m_name);
  }
}

void ProgramPipeline::destroy() {
  if (m_name != 0) {
    glDeleteProgramPipelines(1, &m_name);
    m_name = 0;
  }
}

bool ProgramPipeline::is_valid() const {
  return glIsProgramPipeline(m_name) == GL_TRUE;
}

void ProgramPipeline::bind() const {
  glBindProgramPipeline(m_name);
}

void ProgramPipeline::unbind() {
  glBindProgramPipeline(0);
}

void ProgramPipeline::use_program_stages(GLbitfield stages, GLuint program) const {
  glUseProgramStages(m_name, stages, program);
}

void ProgramPipeline::active_shader_program(GLuint program) const {
  glActiveShaderProgram(m_name, program);
}

bool ProgramPipeline::validate() const {
  glValidateProgramPipeline(m_name);
  return get(GL_VALIDATE_STATUS) == GL_TRUE;
}

template <>
void ProgramPipeline::get<GLint>(GLenum pname, GLint *params) const {
  glGetProgramPipelineiv(m_name, pname, params);
}

GLint ProgramPipeline::get(GLenum pname) const {
  GLint value;
  get<GLint>(pname, &value);
  return value;
}
