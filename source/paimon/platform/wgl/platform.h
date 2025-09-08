#pragma once

#ifdef _WIN32

namespace paimon {

class WglPlatform {
public:
   static WglPlatform& instance();
private:
  WglPlatform();
  ~WglPlatform();
};
} // namespace paimon

#endif // _WIN32