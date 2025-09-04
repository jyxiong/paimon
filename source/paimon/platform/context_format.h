#pragma once

namespace paimon {

class ContextFormat {
  public:
  enum class Profile {
    Core,
    Compatibility,
    None
  };
  public:
    ContextFormat();
    ContextFormat& setVersion(int major, int minor);
    ContextFormat& setProfileCore();
    ContextFormat& setProfileCompatibility();
    ContextFormat& setProfileNone();
    ContextFormat& setForwardCompatible(bool forwardCompatible);
    ContextFormat& setDebugContext(bool debugContext);
    int getMajorVersion() const;
    int getMinorVersion() const;
    bool isForwardCompatible() const;
    bool isDebugContext() const;
    Profile getProfile() const;

private:
    Profile profile;
    int majorVersion;
    int minorVersion;
    bool debugContext;
    bool noErrorContext;
};

} // namespace paimon