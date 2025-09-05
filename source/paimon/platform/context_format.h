#pragma once

namespace paimon {

enum class ContextProfile : unsigned int { NONE, Core, Compatibility };

struct ContextFormat {
  enum class Profile { Core, Compatibility, None };
  unsigned int versionMajor = 0;
  unsigned int versionMinor = 0;
  ContextProfile profile = ContextProfile::NONE;
  bool debug = false;
};

} // namespace paimon