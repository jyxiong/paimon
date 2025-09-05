#include "paimon/platform/context.h"

namespace paimon {

std::unique_ptr<Context> create(const ContextFormat &format);

}