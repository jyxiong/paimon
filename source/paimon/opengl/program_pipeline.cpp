#include "paimon/opengl/program_pipeline.h"

#include "paimon/core/log_system.h"
#include "paimon/opengl/shader_program.h"

using namespace paimon;

ProgramPipeline::ProgramPipeline() : NamedObject(GL_PROGRAM_PIPELINE) {
  glCreateProgramPipelines(1, &m_name);

  if (!validate()) {
    LOG_ERROR("program pipeline error!");

    
  }
}

ProgramPipeline::~ProgramPipeline() {
  if (m_name != 0) {
    glDeleteProgramPipelines(1, &m_name);
  }
}

bool ProgramPipeline::is_valid() const {
  return glIsProgramPipeline(m_name) == GL_TRUE;
}

void ProgramPipeline::bind() const { glBindProgramPipeline(m_name); }

void ProgramPipeline::unbind() { glBindProgramPipeline(0); }

void ProgramPipeline::use_program_stages(GLbitfield stages,
                                         const Program &program) const {
  glUseProgramStages(m_name, stages, program.get_name());
}

void ProgramPipeline::use_program_stages(GLbitfield stages,
                                         const ShaderProgram &program) const {
  glUseProgramStages(m_name, stages, program.get_name());
}

void ProgramPipeline::active_shader_program(const Program &program) const {
  glActiveShaderProgram(m_name, program.get_name());
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
