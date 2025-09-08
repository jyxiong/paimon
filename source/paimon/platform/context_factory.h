#include "paimon/platform/context.h"

#include <memory>

#include "paimon/platform/context_format.h"

namespace paimon {
class ContextFactory {
public:
  static std::unique_ptr<Context> getCurrentContext();

  static std::unique_ptr<Context> createContext(const ContextFormat &format);
};
} // namespace paimon