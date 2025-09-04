#pragma once

#include "paimon/platform/context_format.h"

namespace paimon {

class Context {
public:
Context() = default;
virtual ~Context() = default;

virtual void makeCurrent() = 0;
virtual void doneCurrent() = 0;

private:
  ContextFormat mFormat;
};

} // namespace paimon