#pragma once

namespace paimon {

class Platform {
public:
   static Platform& instance();
private:
  Platform();
  ~Platform();
};
} // namespace paimon