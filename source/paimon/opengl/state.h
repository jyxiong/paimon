#pragma once

#include <unordered_map>
#include <memory>

#include "glad/gl.h"

#include "paimon/opengl/capability.h"

namespace paimon {
class State {
public:
  State();

  ~State() = default;

  void enable(GLenum cap);

  void disable(GLenum cap);

  bool isEnabled(GLenum cap) const;

private:
  std::unordered_map<GLenum, std::unique_ptr<Capability>> m_capabilities;
};
} // namespace paimon