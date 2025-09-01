#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
class ShaderProgram : public NamedObject {
public:
  ShaderProgram(GLenum type, const std::string &source);

  ~ShaderProgram() override;

  ShaderProgram(const ShaderProgram &other) = delete;
  ShaderProgram &operator=(const ShaderProgram &other) = delete;

  bool is_valid() const override;

public:
   void get(GLenum pname, GLint *params) const;
  GLint get(GLenum pname) const;

  void set(GLenum pname, GLint param) const;

  std::string get_info_log() const;

private:
};

} // namespace paimon