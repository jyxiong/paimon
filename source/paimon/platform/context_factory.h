#include "paimon/platform/context.h"

#include <memory>

#include "paimon/platform/context_format.h"

namespace paimon {

std::unique_ptr<Context> createContext(const ContextFormat &format);
}