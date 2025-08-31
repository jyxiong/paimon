#pragma once

#include "paimon/opengl/base/object.h"

namespace paimon {
class SeparateProgram : public NamedObject {
public:
  SeparateProgram(GLenum type, const std::string &source);

private:
};

} // namespace paimon