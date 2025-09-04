#pragma once

namespace paimon {

struct ContextFormat {
  enum class Profile { Core, Compatibility, None };
  Profile profile;
  int majorVersion;
  int minorVersion;
  bool debugContext;
  bool noErrorContext;
};

} // namespace paimon