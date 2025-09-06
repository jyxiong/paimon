#pragma once

namespace paimon {

enum class ContextProfile : unsigned int { NONE, Core, Compatibility };

struct ContextFormat {
  unsigned int versionMajor = 0;
  unsigned int versionMinor = 0;
  ContextProfile profile = ContextProfile::NONE;
  bool debug = false;
};

} // namespace paimon