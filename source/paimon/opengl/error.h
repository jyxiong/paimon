#pragma once

#include <string>

#include "glad/gl.h"

namespace paimon {

class Error {
public:
  Error();
  Error(GLenum errorCode);

  GLenum code() const;
  std::string code_string() const;

  bool isError() const;
  operator bool() const;

public:
  static Error get();

  static void clear();

protected:
  GLenum m_errorCode;
};

} // namespace paimon