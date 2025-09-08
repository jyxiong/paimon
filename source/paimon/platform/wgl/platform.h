#pragma once

#ifdef _WIN32

namespace paimon {

class Platform {
public:
   static Platform& instance();
private:
  Platform();
  ~Platform();
};
} // namespace paimon

#endif // _WIN32