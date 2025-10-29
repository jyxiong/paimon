#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {

class ProgramPipeline : public NamedObject
{
public:
    ProgramPipeline();
    ~ProgramPipeline() override;

    ProgramPipeline(const ProgramPipeline&) = delete;
    ProgramPipeline& operator=(const ProgramPipeline&) = delete;

    void create();

    void destroy();

    bool is_valid() const override;

public:

    void bind() const;
    static void unbind();

    void use_program_stages(GLbitfield stages, GLuint program) const;

    void active_shader_program(GLuint program) const;

    bool validate() const;

    template<typename T>
    void get(GLenum pname, T* params) const;

    GLint get(GLenum pname) const;

private:
};

}