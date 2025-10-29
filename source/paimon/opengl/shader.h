#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
class Shader : public NamedObject {
public:
  Shader(GLenum type);

  ~Shader() override;

  Shader(const Shader &other) = delete;
  Shader &operator=(const Shader &other) = delete;

  void create();

  void destroy();

  bool is_valid() const override;

public:

  void get(GLenum pname, GLint *params) const;
  GLint get(GLenum pname) const;

  std::string get_info_log() const;

  std::string get_source() const;


  GLenum get_type() const;

  bool compile(const std::string &source);

private:
  GLenum m_type;
};

} // namespace paimon