#pragma once

#include "paimon/opengl/base/object.h"
#include "paimon/opengl/shader.h"

namespace paimon {
class Program : public NamedObject {
public:
  Program();

  ~Program() override;

  Program(const Program &other) = delete;
  Program &operator=(const Program &other) = delete;

  void create();

  void destroy();

  bool is_valid() const override;

public:
  void get(GLenum pname, GLint *params) const;
  GLint get(GLenum pname) const;

  void set(GLenum pname, GLint param) const;

  std::string get_info_log() const;

  void attach(const Shader &shader) const;
  void detach(const Shader &shader) const;

  bool link() const;

  void use() const;

private:
};

} // namespace paimon