#pragma once

#include "paimon/opengl/base/object.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader_program.h"

namespace paimon {

class ProgramPipeline : public NamedObject {
public:
  ProgramPipeline();
  ~ProgramPipeline() override;

  ProgramPipeline(const ProgramPipeline &) = delete;
  ProgramPipeline &operator=(const ProgramPipeline &) = delete;

  ProgramPipeline(ProgramPipeline &&other) = default;

  bool is_valid() const override;

public:
  void bind() const;
  static void unbind();

  void use_program_stages(GLbitfield stages, const Program &program) const;
  void use_program_stages(GLbitfield stages,
                          const ShaderProgram &program) const;

  void active_shader_program(const Program &program) const;

  bool validate() const;

  template <typename T>
  void get(GLenum pname, T *params) const;

  GLint get(GLenum pname) const;

private:
};

} // namespace paimon